#ifndef UTILS_BYTES_HPP
#define UTILS_BYTES_HPP


#ifdef LITTLE_ENDIAN 
	inline consteval std::uint32_t read_u32(const uint8_t* p) {
		return static_cast<uint32_t>(p[0])
			| (static_cast<uint32_t>(p[1]) << 8)
			| (static_cast<uint32_t>(p[2]) << 16)
			| (static_cast<uint32_t>(p[3]) << 24);
	}

	inline consteval std::uint16_t read_u16(const uint8_t* p) {
		return static_cast<uint16_t>(p[0]) 
			| (static_cast<uint16_t>(p[1]) << 8);
	}
#elif defined(BIG_ENDIAN)
	inline consteval std::uint32_t read_u32(const uint8_t* p) {
		return (static_cast<uint32_t>(p[0]) << 24)
			|  (static_cast<uint32_t>(p[1]) << 16)
			|  (static_cast<uint32_t>(p[2]) << 8)
			|   static_cast<uint32_t>(p[3]);
	}

	inline consteval std::uint16_t read_u16(const uint8_t* p) {
		return (static_cast<uint16_t>(p[0]) << 8) 
			|   static_cast<uint16_t>(p[1]);
	}
#endif


#endif // UTILS_BYTES_HPP