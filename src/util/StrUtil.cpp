#include "schedule/util/StrUtil.h"

namespace schedule {
namespace strutil {

std::string trim(const std::string& text) {
    const std::size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return std::string();
    }
    const std::size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::string              current;
    for (const char c : text) {
        if (c == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    parts.push_back(current);
    return parts;
}

bool tryParseInt(const std::string& text, int& out) {
    const std::string trimmed = trim(text);
    if (trimmed.empty()) {
        return false;
    }
    std::size_t i = 0;
    int         sign = 1;
    if (trimmed[0] == '+' || trimmed[0] == '-') {
        sign = (trimmed[0] == '-') ? -1 : 1;
        i    = 1;
        if (trimmed.size() == 1) {
            return false;
        }
    }
    long long value = 0;
    for (; i < trimmed.size(); ++i) {
        const char c = trimmed[i];
        if (c < '0' || c > '9') {
            return false;
        }
        value = value * 10 + (c - '0');
        if (value > 100000000LL) {       // 防止疯长的输入
            return false;
        }
    }
    out = static_cast<int>(value) * sign;
    return true;
}

std::string replaceAll(const std::string& text, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return text;
    }
    std::string        result;
    std::size_t        pos  = 0;
    while (true) {
        const std::size_t hit = text.find(from, pos);
        if (hit == std::string::npos) {
            result.append(text, pos, std::string::npos);
            break;
        }
        result.append(text, pos, hit - pos);
        result += to;
        pos = hit + from.size();
    }
    return result;
}

std::size_t displayWidth(const std::string& text) {
    // 简化处理：UTF-8 里以 0xE0/0xE8 等开头的 3 字节字符（中日韩文字）算 2 格，
    // 其余算 1 格。够用了，这里不需要百分百精确。
    std::size_t width = 0;
    for (std::size_t i = 0; i < text.size();) {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        if ((c & 0x80) == 0) {                 // ASCII
            ++width;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {        // 2 字节
            ++width;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {        // 3 字节：常见汉字
            width += 2;
            i += 3;
        } else {                                // 4 字节：emoji 等
            width += 2;
            i += 4;
        }
    }
    return width;
}

}  // namespace strutil
}  // namespace schedule
