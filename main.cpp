#include <iostream>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
}

// convert c++11 enum class to int
template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

std::string conf_info() {
    return avcodec_configuration();
}

std::string proto_info() {
    std::string info;

    enum class ProtocolDirection {
        INPUT = 0,
        OUTPUT = 1
    };

    struct DummyProtocol;
    DummyProtocol* ptr = nullptr;
    DummyProtocol** pptr = &ptr;

    // enum input protocols
    avio_enum_protocols((void**)pptr, to_underlying(ProtocolDirection::INPUT));
    while (*pptr) {
        const char* proto_name =
            avio_enum_protocols((void**)pptr, to_underlying(ProtocolDirection::INPUT));
        if (proto_name) {
            info.append("[IN PROTOCOL][");
            info.append(proto_name);
            info.append("]\n");
        }
    }

    // enum output protocols
    ptr = nullptr;
    avio_enum_protocols((void**)pptr, to_underlying(ProtocolDirection::OUTPUT));
    while (*pptr) {
        const char* proto_name =
            avio_enum_protocols((void**)pptr, to_underlying(ProtocolDirection::OUTPUT));
        if (proto_name) {
            info.append("[OUT PROTOCOL][");
            info.append(proto_name);
            info.append("]\n");
        }
    }

    return info;
}

std::string avformat_info() {
    std::string info;

    // enum input format
    AVInputFormat* iformat = av_iformat_next(nullptr);
    while (iformat) {
        info.append("[IN FORMAT][");
        info.append(iformat->name);
        info.append("]\n");

        iformat = av_iformat_next(iformat);
    }

    // enum output format
    AVOutputFormat* oformat = av_oformat_next(nullptr);
    while (oformat) {
        info.append("[OUT FORMAT][");
        info.append(oformat->name);
        info.append("]\n");

        oformat = av_oformat_next(oformat);
    }
    return info;
}

std::string avcodec_info() {
    std::string info;

    AVCodec* state = nullptr;
    AVCodec** pstate = &state;

    const AVCodec* codec = av_codec_iterate((void**)pstate);
    while (codec) {
        if (codec->decode) {
            info.append("[Dec]");
        } else {
            info.append("[Enc]");
        }

        switch (codec->type) {
            case AVMEDIA_TYPE_VIDEO:
                info.append("[VIDEO]");
                break;
            case AVMEDIA_TYPE_AUDIO:
                info.append("[AUDIO]");
                break;
            default:
                info.append("[OTHER]");
                break;
        }

        info.append(codec->name);
        info.append("\n");

        codec = av_codec_iterate((void**)pstate);
    }

    return info;
}

std::string avfilter_info() {
    std::string info;

    AVFilter* state = nullptr;
    AVFilter** pstate = &state;

    const AVFilter* filter = av_filter_iterate((void**)pstate);
    while (filter) {
        info.append("[Filter]");
        info.append(filter->name);
        info.append("\n");

        filter = av_filter_iterate((void**)pstate);
    }

    return info;
}

int main() {
    std::cout << "Configuration: \n"
        << conf_info() << std::endl;

    std::cout << "Protocol: \n"
        << proto_info() << std::endl;

    std::cout << "AvFormat: \n"
        << avformat_info() << std::endl;

    std::cout << "AvCodec: \n"
        << avcodec_info() << std::endl;

    std::cout << "AvFilter: \n"
        << avfilter_info() << std::endl;
    return 0;
}
