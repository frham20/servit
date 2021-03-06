#include "servit.h"

#include <cstdio>
#include <vector>

static servit::thread_pool s_threadPool( servit::thread_pool::DEFAULT_MAX_THREADS );
static servit::tcp_socket s_listenSocket;
static std::vector<AcceptSocket> s_acceptSockets;
static std::vector<size_t> s_acceptSocketFreeList;
static size_t s_maxConcurrentConnections = 256;
static PTP_IO s_listenSocketIO = nullptr;

constexpr size_t ADDRESS_SIZE = sizeof( sockaddr_in );
constexpr size_t ACCEPT_OUTPUT_BUFFER_SIZE = ( ADDRESS_SIZE + 16 ) * 2;
constexpr size_t RECEIVE_BUFFER_SIZE = 4096;

struct AcceptSocket
{
    servit::tcp_socket socket;
    size_t index = static_cast<size_t>( -1 );
    char acceptOutputBuffer[ACCEPT_OUTPUT_BUFFER_SIZE] = { 0 };
    char receiveBuffer[RECEIVE_BUFFER_SIZE] = { 0 };
};


static void CALLBACK ListenSocketIoCompletionCallback( PTP_CALLBACK_INSTANCE instance, PVOID context, PVOID overlapped, ULONG ioResult, ULONG_PTR numberOfBytesTransferred, PTP_IO io )
{
    AcceptSocket* as = reinterpret_cast<AcceptSocket*>( overlapped );

    instance;
    context;
    overlapped;
    ioResult;
    numberOfBytesTransferred;
    io;
    //TODO

    as->socket.read_async( as->receiveBuffer, sizeof( as->receiveBuffer ), AcceptSocketIoCompletionCallback );


    //queue read operation
    ::StartThreadpoolIo( as->io );
    WSABUF recvBuffer = { 0 };
    recvBuffer.buf = as->receiveBuffer;
    recvBuffer.len = sizeof( as->receiveBuffer );
    DWORD receiveFlags = 0;
    if ( ::WSARecv( as->handle, &recvBuffer, 1, nullptr, &receiveFlags, &as->overlapped, nullptr ) == SOCKET_ERROR )
    {
        const int wsaError = ::WSAGetLastError();
        if ( wsaError != WSA_IO_PENDING )
        {
            ::CancelThreadpoolIo( io );
        }
    }
}

static void CALLBACK AcceptSocketIoCompletionCallback( PTP_CALLBACK_INSTANCE instance, PVOID context, PVOID overlapped, ULONG ioResult, ULONG_PTR numberOfBytesTransferred, PTP_IO io )
{
    AcceptSocket* as = reinterpret_cast<AcceptSocket*>( overlapped );

    instance;
    context;
    overlapped;
    ioResult;
    numberOfBytesTransferred;
    io;
    //TODO

    //queue next read opertation

    //queue read operation
    ::StartThreadpoolIo( as->io );
    WSABUF recvBuffer = { 0 };
    recvBuffer.buf = as->receiveBuffer;
    recvBuffer.len = sizeof( as->receiveBuffer );
    DWORD receiveFlags = 0;
    if ( ::WSARecv( as->handle, &recvBuffer, 1, nullptr, &receiveFlags, &as->overlapped, nullptr ) == SOCKET_ERROR )
    {
        const int wsaError = ::WSAGetLastError();
        if ( wsaError != WSA_IO_PENDING )
        {
            ::CancelThreadpoolIo( io );
        }
    }
}

static AcceptSocket* AllocAcceptSocket()
{
    if ( s_acceptSockets.size() - s_acceptSocketFreeList.size() >= s_maxConcurrentConnections )
        return nullptr;

    SOCKET socket = ::WSASocketW( AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED );
    if ( socket == INVALID_SOCKET )
        return nullptr;

    AcceptSocket* as = nullptr;
    if ( s_acceptSocketFreeList.empty() )
    {
        s_acceptSockets.emplace_back( AcceptSocket() );
        as = &s_acceptSockets.back();
        as->index = s_acceptSockets.size() - 1;
    }
    else
    {
        as = &s_acceptSockets[s_acceptSocketFreeList.back()];
        s_acceptSocketFreeList.pop_back();
    }

    as->handle = socket;

    PTP_IO io = ::CreateThreadpoolIo( reinterpret_cast<HANDLE>( socket ), AcceptSocketIoCompletionCallback, as, &s_threadPool.get_imp().get_env() );
    if ( io == nullptr )
    {
        ::closesocket( socket );
        return nullptr;
    }

    as->io = io;

    //listen for incoming connection
    ::StartThreadpoolIo( s_listenSocketIO );
    DWORD bytesReceived;
    if ( ::AcceptEx( s_listenSocket.get_imp().get_handle(), as->handle, as->acceptOutputBuffer, 0, ADDRESS_SIZE + 16, ADDRESS_SIZE + 16, &bytesReceived, &as->overlapped ) != TRUE )
    {
        const int wsaError = ::WSAGetLastError();
        if ( wsaError != WSA_IO_PENDING )
        {
            ::CancelThreadpoolIo( s_listenSocketIO );
            ::closesocket( socket );
            return nullptr;
        }
    }

    //inherit accept socket from listen socket
    auto listenHandle = s_listenSocket.get_imp().get_handle();
    ::setsockopt( as->handle, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>( &listenHandle ), sizeof( listenHandle ) );

    return as;
}

static void FreeAcceptSocket( AcceptSocket* as )
{
    //cancel any pending io
    if ( as->io != nullptr )
    {
        ::CancelThreadpoolIo( as->io );
        as->io = nullptr;
    }

    //need to wait before closing socket?
    //TBD

    //close socket
    if ( as->handle != INVALID_SOCKET )
    {
        ::closesocket( as->handle );
        as->handle = INVALID_SOCKET;
    }

    //add to free list
    s_acceptSocketFreeList.emplace_back( as->index );
}

bool Server_Init()
{
    printf( "Initializing Server\n" );

    //create listen socket
    if ( !s_listenSocket.create() )
    {
        printf( "Error creating listening socket\n" );
        return false;
    }

    s_listenSocketIO = ::CreateThreadpoolIo( reinterpret_cast<HANDLE>( s_listenSocket.get_imp().get_handle() ), ListenSocketIoCompletionCallback, 0, &s_threadPool.get_imp().get_env() );
    if ( s_listenSocketIO == nullptr )
    {
        return false;
    }

    //bind local address to socket
    if ( !s_listenSocket.bind( 80 ) )
    {
        printf( "Error binding socket to port 80\n" );
        return false;
    }

    //start listening
    if ( !s_listenSocket.listen() )
    {
        printf( "Error listening on port 80\n" );
        return false;
    }

    return true;
}

void Server_Close()
{
    printf( "Cleaning up\n" );

    //close all sockets
    for ( auto& as : s_acceptSockets )
    {
        FreeAcceptSocket( &as );
    }

    s_listenSocket.close();
}

void Server_Listen()
{
    while ( true )
    {
        auto as = AllocAcceptSocket();
        if ( as == nullptr )
        {
            //let's sleep for a duration for now, we can do better than this though using an event
            ::SleepEx( 500, FALSE );
            continue;
        }
    }
}

int main()
{
    if ( !servit::init() )
        return 1;

    if ( !Server_Init() )
    {
        Server_Close();
        return 1;
    }

    Server_Listen();

    Server_Close();

    servit::close();

    return 0;
}

