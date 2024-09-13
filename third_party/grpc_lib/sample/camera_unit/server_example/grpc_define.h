#pragma once

enum
{
    GRPC_FHD_RGB24_LENGTH = (1920 * 1080 * 3),

    // GRPC_BUFFER_PADDING_SIZE 사이즈는 PacketWrite, PacketReader 할시에 비디오 스트림 데이터를 제외한
    // 메세지 길이를 나타낸다.
    GRPC_BUFFER_PADDING_SIZE = 512,

    // clinet -> ChannelArguments::SetMaxReceiveMessageSize() 함수 참조
    // server -> ServerBuilder::SetMaxReceiveMessageSize() 함수 참조
    GRPC_CLIENT_RECV_BUFFER_SIZE = GRPC_FHD_RGB24_LENGTH + GRPC_BUFFER_PADDING_SIZE,
};