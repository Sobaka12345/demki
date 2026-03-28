#ifndef PLAIN_VIEW_HPP
#define PLAIN_VIEW_HPP

namespace resources {

template <typename BufferT>
struct PlainView
{
    BufferT buffer;

    size_t offset = 0;
    size_t size() const noexcept { return buffer.size(); }

    void write(const void *data, uint64_t size) noexcept {
        buffer.write(data, size, offset);
        offset += size;
    }
};

}

#endif // PLAIN_VIEW_HPP