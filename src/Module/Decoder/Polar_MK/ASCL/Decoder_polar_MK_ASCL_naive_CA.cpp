#include <string>

#include "Module/Decoder/Polar_MK/ASCL/Decoder_polar_MK_ASCL_naive_CA.hpp"

using namespace aff3ct;
using namespace aff3ct::module;

template <typename B, typename R>
Decoder_polar_MK_ASCL_naive_CA<B,R>
::Decoder_polar_MK_ASCL_naive_CA(const int& K,
                                 const int& N,
                                 const int& L,
                                 const tools::Polar_code& code,
                                 const std::vector<bool>& frozen_bits,
                                 const std::vector<std::vector<std::function<R(const std::vector<R> &LLRs,
                                                                               const std::vector<B> &bits)>>> &lambdas,
                                 const CRC<B>& crc,
                                 const int n_frames)
: Decoder_polar_MK_SCL_naive_CA<B,R>(K, N, L, code, frozen_bits, lambdas, crc, n_frames),
  sc_decoder(new Decoder_polar_MK_SC_naive<B,R>(K, N, code, frozen_bits, lambdas, n_frames))
{
	const std::string name = "Decoder_polar_MK_ASCL_naive_CA";
	this->set_name(name);
}

template <typename B, typename R>
Decoder_polar_MK_ASCL_naive_CA<B,R>
::Decoder_polar_MK_ASCL_naive_CA(const int& K,
                                 const int& N,
                                 const int& L,
                                 const tools::Polar_code& code,
                                 const std::vector<bool>& frozen_bits,
                                 const CRC<B>& crc,
                                 const int n_frames)
: Decoder_polar_MK_ASCL_naive_CA<B,R>(K, N, L, code, frozen_bits, {}, crc, n_frames)
{
}

template <typename B, typename R>
Decoder_polar_MK_ASCL_naive_CA<B,R>* Decoder_polar_MK_ASCL_naive_CA<B,R>
::clone() const
{
	auto m = new Decoder_polar_MK_ASCL_naive_CA(*this);
	m->deep_copy(*this);
	return m;
}

template <typename B, typename R>
void Decoder_polar_MK_ASCL_naive_CA<B,R>
::deep_copy(const Decoder_polar_MK_ASCL_naive_CA<B,R> &m)
{
	Decoder_polar_MK_SCL_naive_CA<B,R>::deep_copy(m);
	if (m.sc_decoder != nullptr) this->sc_decoder.reset(m.sc_decoder->clone());
}

template <typename B, typename R>
void Decoder_polar_MK_ASCL_naive_CA<B,R>
::notify_noise_update()
{
	Decoder_polar_MK_SCL_naive_CA<B,R>::notify_noise_update();
	sc_decoder->notify_noise_update();
}

template <typename B, typename R>
int Decoder_polar_MK_ASCL_naive_CA<B,R>
::_decode_siho(const R *Y_N, B *V_K, const int frame_id)
{
	sc_decoder->_decode_siho(Y_N, V_K, frame_id);

	// check the CRC
	auto crc_decode_result = this->crc->check(V_K, this->get_simd_inter_frame_level());

	if (!crc_decode_result)
		Decoder_polar_MK_SCL_naive_CA<B,R>::_decode_siho(Y_N, V_K, frame_id);

	return 0;
}

template <typename B, typename R>
int Decoder_polar_MK_ASCL_naive_CA<B,R>
::_decode_siho_cw(const R *Y_N, B *V_N, const int frame_id)
{
	sc_decoder->_decode_siho(Y_N, V_N, frame_id);

	// check the CRC
	auto crc_decode_result = this->crc->check(V_N, this->get_simd_inter_frame_level());

	if (!crc_decode_result)
		Decoder_polar_MK_SCL_naive_CA<B,R>::_decode_siho_cw(Y_N, V_N, frame_id);
	else
		sc_decoder->_store(V_N, true);

	return 0;
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::module::Decoder_polar_MK_ASCL_naive_CA<B_8,Q_8>;
template class aff3ct::module::Decoder_polar_MK_ASCL_naive_CA<B_16,Q_16>;
template class aff3ct::module::Decoder_polar_MK_ASCL_naive_CA<B_32,Q_32>;
template class aff3ct::module::Decoder_polar_MK_ASCL_naive_CA<B_64,Q_64>;
#else
template class aff3ct::module::Decoder_polar_MK_ASCL_naive_CA<B,Q>;
#endif
// ==================================================================================== explicit template instantiation