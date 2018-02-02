#ifndef DECODER_TURBO_PRODUCT_HPP_
#define DECODER_TURBO_PRODUCT_HPP_

#include "Module/Interleaver/Interleaver.hpp"

#include "../Decoder_SISO_SIHO.hpp"
#include "../Decoder_HIHO.hpp"
#include "../../Encoder/Encoder.hpp"

namespace aff3ct
{
namespace module
{

template <typename B = int, typename R = float>
class Decoder_turbo_product : public Decoder_SISO_SIHO<B,R>
{
protected:
	const int  n_ite; // number of iterations
	const Interleaver<R> &pi;
	Decoder_HIHO<B> &dec_r; // row decoder
	Decoder_HIHO<B> &dec_c; // col decoder
    Encoder     <B> &enc_r; // row encoder
    Encoder     <B> &enc_c; // col encoder

	const bool parity_extended;

	std::vector<R> Y_N_i;
	std::vector<R> Y_N_pi;
	std::vector<B> V_K_i;
	std::vector<B> V_N_i;

public:
	Decoder_turbo_product(const int& n_ite,
	                      const Interleaver<R> &pi,
	                      Decoder_HIHO<B> &dec_r,
	                      Decoder_HIHO<B> &dec_c,
	                      Encoder     <B> &enc_r,
	                      Encoder     <B> &enc_c);
	virtual ~Decoder_turbo_product();

protected:
	void _decode_siso   (const R *Y_N1, R *Y_N2, const int frame_id);
	void _decode_siho   (const R *Y_N,  B *V_K,  const int frame_id);
	void _decode_siho_cw(const R *Y_N,  B *V_N,  const int frame_id);

	virtual void _decode(const R *Y_N, int return_K_siso) = 0;
};

}
}

#endif /* DECODER_TURBO_PRODUCT_HPP_ */
