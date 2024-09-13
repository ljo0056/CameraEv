#include "stdafx.h"
#include "PacketStruct.h"
#include "PacketDefine.h"
#include "decoder/DecoderDefine.h"

#include <string>

//////////////////////////////////////////////////////////////////////////
#define PTOS(x)  (#x)

std::shared_ptr<PacketStruct> PacketStruct::CreateFactory(const std::string& name)
{
    std::shared_ptr<PacketStruct> ptr;

    if (PTOS(PACKEK_TYPE_FRAME) == name)
        ptr = std::make_shared<Packet_Frame>();
    else if (PTOS(PACKEK_TYPE_PROCESS_INFO_REQ) == name)
        ptr = std::make_shared<Packet_ProcessInfoReq>();
    else if (PTOS(PACKEK_TYPE_PROCESS_INFO) == name)
        ptr = std::make_shared<Packet_ProcessInfo>();
    else if (PTOS(PACKEK_TYPE_STREAM_INFO) == name)
        ptr = std::make_shared<Packet_StreamInfo>();
    else if (PTOS(PACKEK_TYPE_STREAM_REQUEST) == name)
        ptr = std::make_shared<Packet_StreamRequest>();
    else if (PTOS(PACKEK_TYPE_STREAM_STATUS) == name)
        ptr = std::make_shared<Packet_StreamStatus>();

    return ptr;
}

std::shared_ptr<PacketStruct> PacketStruct::CreateFactory(PACKEK_TYPE type)
{
    std::shared_ptr<PacketStruct> ptr;

    if (PACKEK_TYPE_FRAME == type)
        ptr = std::make_shared<Packet_Frame>();
    else if (PACKEK_TYPE_PROCESS_INFO_REQ == type)
        ptr = std::make_shared<Packet_ProcessInfoReq>();
    else if (PACKEK_TYPE_PROCESS_INFO == type)
        ptr = std::make_shared<Packet_ProcessInfo>();
    else if (PACKEK_TYPE_STREAM_INFO == type)
        ptr = std::make_shared<Packet_StreamInfo>();
    else if (PACKEK_TYPE_STREAM_REQUEST == type)
        ptr = std::make_shared<Packet_StreamRequest>();
    else if (PACKEK_TYPE_STREAM_STATUS == type)
        ptr = std::make_shared<Packet_StreamStatus>();

    return ptr;
}

//////////////////////////////////////////////////////////////////////////

PACKEK_TYPE Packet_Frame::PacketType()
{
    return PACKEK_TYPE_FRAME;
}

std::string Packet_Frame::PacketTypeStr()
{
    return PTOS(PACKEK_TYPE_FRAME);
}

int Packet_Frame::Writer(CPacketWriter* writer)
{
    writer->Start(PacketTypeStr());
    writer->Write("GUID"         , camera_guid);
    writer->Write("StreamIndex"  , stream_index);
    writer->Write("OrderNum"     , order_num);
    writer->Write("SWidth"       , stream_width);
    writer->Write("SHeight"      , stream_height);
    writer->Write("FrameType"    , frame_type);
    writer->Write("FPixelFormat" , frame_pixel_format);
    writer->Write("FWidth"       , frame_width);
    writer->Write("FHeight"      , frame_height);
    writer->Write("VCodecId"     , vcodec_id);
    writer->Write("FrameData"    , frame_buffer, frame_buffer_len);
    writer->End();

    return 0;
}

int Packet_Frame::Reader(CPacketReader* reader)
{
    reader->Read("GUID"        , camera_guid);
    reader->Read("StreamIndex" , stream_index);
    reader->Read("OrderNum"    , order_num);
    reader->Read("SWidth"      , stream_width);
    reader->Read("SHeight"     , stream_height);
    reader->Read("FrameType"   , frame_type);
    reader->Read("FPixelFormat", frame_pixel_format);
    reader->Read("FWidth"      , frame_width);
    reader->Read("FHeight"     , frame_height);
    reader->Read("VCodecId"    , vcodec_id);

    frame_buffer_len = reader->ReadLength("FrameData");
    reader->Read("FrameData", frame_buffer);

    return 0;
}

std::shared_ptr<Packet_Frame> Packet_Frame::Casting(std::shared_ptr<PacketStruct> parent)
{
    return std::dynamic_pointer_cast<Packet_Frame>(parent);
}

//////////////////////////////////////////////////////////////////////////

PACKEK_TYPE Packet_ProcessInfoReq::PacketType()
{
    return PACKEK_TYPE_PROCESS_INFO_REQ;
}

std::string Packet_ProcessInfoReq::PacketTypeStr()
{
    return PTOS(PACKEK_TYPE_PROCESS_INFO_REQ);
}

int Packet_ProcessInfoReq::Writer(CPacketWriter* writer)
{
    writer->Start(PacketTypeStr());
    writer->Write("QueueName", queue_name);
    writer->Write("SyncCount", sync_count);
    writer->End();

    return 0;
}

int Packet_ProcessInfoReq::Reader(CPacketReader* reader)
{
    reader->Read("QueueName", queue_name);
    reader->Read("SyncCount", sync_count);

    return 0;
}

std::shared_ptr<Packet_ProcessInfoReq> Packet_ProcessInfoReq::Casting(std::shared_ptr<PacketStruct> parent)
{
    return std::dynamic_pointer_cast<Packet_ProcessInfoReq>(parent);
}


PACKEK_TYPE Packet_ProcessInfo::PacketType()
{
    return PACKEK_TYPE_PROCESS_INFO;
}

std::string Packet_ProcessInfo::PacketTypeStr()
{
    return PTOS(PACKEK_TYPE_PROCESS_INFO);
}

int Packet_ProcessInfo::Writer(CPacketWriter* writer)
{
    writer->Start(PacketTypeStr());
    writer->Write("Name", process_name);
    writer->Write("PID" , process_id);
    writer->Write("PacketVersion", packet_version);
    writer->End();

    return 0;
}

int Packet_ProcessInfo::Reader(CPacketReader* reader)
{
    reader->Read("Name", process_name);
    reader->Read("PID" , process_id);
    reader->Read("PacketVersion", packet_version);

    return 0;
}

std::shared_ptr<Packet_ProcessInfo> Packet_ProcessInfo::Casting(std::shared_ptr<PacketStruct> parent)
{
    return std::dynamic_pointer_cast<Packet_ProcessInfo>(parent);
}

//////////////////////////////////////////////////////////////////////////

PACKEK_TYPE Packet_StreamInfo::PacketType()
{
    return PACKEK_TYPE_STREAM_INFO;
}

std::string Packet_StreamInfo::PacketTypeStr()
{
    return PTOS(PACKEK_TYPE_STREAM_INFO);
}

int Packet_StreamInfo::Writer(CPacketWriter* writer)
{
    writer->Start(PacketTypeStr());
    writer->Write("SetupFile"   , setup_file);
    writer->End();

    return 0;
}

int Packet_StreamInfo::Reader(CPacketReader* reader)
{
    reader->Read("SetupFile"   , setup_file);

    return 0;
}

std::shared_ptr<Packet_StreamInfo> Packet_StreamInfo::Casting(std::shared_ptr<PacketStruct> parent)
{
    return std::dynamic_pointer_cast<Packet_StreamInfo>(parent);
}

//////////////////////////////////////////////////////////////////////////

PACKEK_TYPE Packet_StreamRequest::PacketType()
{
    return PACKEK_TYPE_STREAM_REQUEST;
}

std::string Packet_StreamRequest::PacketTypeStr()
{
    return PTOS(PACKEK_TYPE_STREAM_REQUEST);
}

int Packet_StreamRequest::Writer(CPacketWriter* writer)
{
    writer->Start(PacketTypeStr());
    writer->Write("QueueName"       , queue_name);
    writer->Write("StreamIndex"     , stream_index);
    writer->Write("VideoStreaming"  , video_streaming);
    writer->Write("FrameType"       , frame_type);
    writer->Write("PixelFormat"     , pixel_format);
    writer->Write("Width"           , width);
    writer->Write("Height"          , height);
    writer->End();

    return 0;
}

int Packet_StreamRequest::Reader(CPacketReader* reader)
{
    reader->Read("QueueName"        , queue_name);
    reader->Read("StreamIndex"      , stream_index);
    reader->Read("VideoStreaming"   , video_streaming);
    reader->Read("FrameType"        , frame_type);
    reader->Read("PixelFormat"      , pixel_format);
    reader->Read("Width"            , width);
    reader->Read("Height"           , height);
    return 0;
}

std::shared_ptr<Packet_StreamRequest> Packet_StreamRequest::Casting(std::shared_ptr<PacketStruct> parent)
{
    return std::dynamic_pointer_cast<Packet_StreamRequest>(parent);
}

//////////////////////////////////////////////////////////////////////////

PACKEK_TYPE Packet_StreamStatus::PacketType()
{
    return PACKEK_TYPE_STREAM_STATUS;
}

std::string Packet_StreamStatus::PacketTypeStr()
{
    return PTOS(PACKEK_TYPE_STREAM_STATUS);
}

int Packet_StreamStatus::Writer(CPacketWriter* writer)
{
    writer->Start(PacketTypeStr());
    writer->Write("StreamIndex", stream_index);
    writer->Write("Status", status);
    writer->End();

    return 0;
}

int Packet_StreamStatus::Reader(CPacketReader* reader)
{
    reader->Read("StreamIndex", stream_index);
    reader->Read("Status", status);

    return 0;
}

std::shared_ptr<Packet_StreamStatus> Packet_StreamStatus::Casting(std::shared_ptr<PacketStruct> parent)
{
    return std::dynamic_pointer_cast<Packet_StreamStatus>(parent);
}

//////////////////////////////////////////////////////////////////////////