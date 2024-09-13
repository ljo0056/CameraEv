#pragma once

#include <vector>
#include <memory>

class CPacketWriter;
class CPacketReader;

enum PACKEK_TYPE
{
    PACKEK_TYPE_NONE  = 0,
    PACKEK_TYPE_FRAME,
    PACKEK_TYPE_PROCESS_INFO_REQ,
    PACKEK_TYPE_PROCESS_INFO,
    PACKEK_TYPE_STREAM_INFO,
    PACKEK_TYPE_STREAM_REQUEST,
    PACKEK_TYPE_STREAM_STATUS,
};

//////////////////////////////////////////////////////////////////////////

class PacketStruct
{
public:
    PACKEK_TYPE type = PACKEK_TYPE_NONE;

public:
    PacketStruct() = default;
    virtual ~PacketStruct() {}

    virtual PACKEK_TYPE PacketType() = 0;
    virtual std::string PacketTypeStr() = 0;
    virtual int Writer(CPacketWriter* writer) = 0;
    virtual int Reader(CPacketReader* reader) = 0;

    static std::shared_ptr<PacketStruct> CreateFactory(const std::string& name);
    static std::shared_ptr<PacketStruct> CreateFactory(PACKEK_TYPE type);
};

//////////////////////////////////////////////////////////////////////////

class Packet_Frame : public PacketStruct
{
public:
    std::string camera_guid;

    int stream_index        = 0;
    int order_num           = 0;    // decoder 에서 정해주는 frame 순서
    int stream_width        = 0;    // stream 에서 들어오는 원본 해상도
    int stream_height       = 0;    // stream 에서 들어오는 원본 해상도
    int frame_type          = 0;    // enum FRAME_TYPE
    int frame_pixel_format  = 0;    // enum VIDEO_PIXEL_FORMAT
    int frame_width         = 0;
    int frame_height        = 0;
    int vcodec_id           = 0;    // enum VCODEC_ID

    uint8_t* frame_buffer = nullptr;
    int frame_buffer_len  = 0;

public:
    Packet_Frame() = default;
    virtual ~Packet_Frame() {}

    virtual PACKEK_TYPE PacketType() override;
    virtual std::string PacketTypeStr() override;
    virtual int Writer(CPacketWriter* writer) override;
    virtual int Reader(CPacketReader* reader) override;

    static std::shared_ptr<Packet_Frame> Casting(std::shared_ptr<PacketStruct> parent);
};

typedef std::shared_ptr<Packet_Frame> Packet_FramePtr;

//////////////////////////////////////////////////////////////////////////

class Packet_ProcessInfoReq : public PacketStruct
{
public:
    std::string queue_name;
    int         sync_count = 3;

public:
    Packet_ProcessInfoReq() = default;
    virtual ~Packet_ProcessInfoReq() {}

    virtual PACKEK_TYPE PacketType() override;
    virtual std::string PacketTypeStr() override;
    virtual int Writer(CPacketWriter* writer) override;
    virtual int Reader(CPacketReader* reader) override;

    static std::shared_ptr<Packet_ProcessInfoReq> Casting(std::shared_ptr<PacketStruct> parent);
};

typedef std::shared_ptr<Packet_ProcessInfoReq> Packet_ProcessInfoReqPtr;


class Packet_ProcessInfo : public PacketStruct
{
public:
    std::string process_name;
    int         process_id = 0;     // pid
    std::string packet_version;

public:
    Packet_ProcessInfo() = default;
    virtual ~Packet_ProcessInfo() {}

    virtual PACKEK_TYPE PacketType() override;
    virtual std::string PacketTypeStr() override;
    virtual int Writer(CPacketWriter* writer) override;
    virtual int Reader(CPacketReader* reader) override;

    static std::shared_ptr<Packet_ProcessInfo> Casting(std::shared_ptr<PacketStruct> parent);
};

typedef std::shared_ptr<Packet_ProcessInfo> Packet_ProcessInfoPtr;

//////////////////////////////////////////////////////////////////////////

class Packet_StreamInfo : public PacketStruct
{
public:
    std::string setup_file;

public:
    Packet_StreamInfo() = default;
    virtual ~Packet_StreamInfo() {}

    virtual PACKEK_TYPE PacketType() override;
    virtual std::string PacketTypeStr() override;
    virtual int Writer(CPacketWriter* writer) override;
    virtual int Reader(CPacketReader* reader) override;

    static std::shared_ptr<Packet_StreamInfo> Casting(std::shared_ptr<PacketStruct> parent);
};

typedef std::shared_ptr<Packet_StreamInfo> Packet_StreamInfoPtr;

//////////////////////////////////////////////////////////////////////////

class Packet_StreamRequest : public PacketStruct
{
public:
    std::string queue_name;
    int         stream_index = 0;
    bool        video_streaming = true;

    int         frame_type   = 0;    // enum FRAME_TYPE
    int         pixel_format = 0;
    int         width        = 0;
    int         height       = 0;

public:
    Packet_StreamRequest() = default;
    virtual ~Packet_StreamRequest() {}

    virtual PACKEK_TYPE PacketType() override;
    virtual std::string PacketTypeStr() override;
    virtual int Writer(CPacketWriter* writer) override;
    virtual int Reader(CPacketReader* reader) override;

    static std::shared_ptr<Packet_StreamRequest> Casting(std::shared_ptr<PacketStruct> parent);
};

typedef std::shared_ptr<Packet_StreamRequest> Packet_StreamRequestPtr;

//////////////////////////////////////////////////////////////////////////

class Packet_StreamStatus : public PacketStruct
{
public:
    enum STATUS
    {
        NONE = 0,
        READY,
        RECONNECT,
        NO_SIGNAL,
    };

    int         stream_index = 0;
    int         status = NONE;

public:
    Packet_StreamStatus() = default;
    virtual ~Packet_StreamStatus() {}

    virtual PACKEK_TYPE PacketType() override;
    virtual std::string PacketTypeStr() override;
    virtual int Writer(CPacketWriter* writer) override;
    virtual int Reader(CPacketReader* reader) override;

    static std::shared_ptr<Packet_StreamStatus> Casting(std::shared_ptr<PacketStruct> parent);
};

typedef std::shared_ptr<Packet_StreamStatus> Packet_StreamStatusPtr;

//////////////////////////////////////////////////////////////////////////