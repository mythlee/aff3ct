#ifndef DECODER_CHASE_PYNDIAH_HPP_
#define	DECODER_CHASE_PYNDIAH_HPP_

#include <aff3ct.hpp>
#include <vector>

#include "../Decoder_turbo_product.hpp"

namespace aff3ct
{
namespace module
{

template <typename B = int, typename R = float>
class Decoder_chase_pyndiah : public Decoder_turbo_product<B,R>
{
protected:

	struct info
	{
		R metric;
		int pos;
	};

	 R   alpha;
	const int n_least_reliable_positions;

	std::vector<info> least_reliable_pos;
	std::vector<B>    hard_Rprime;
	B parity_diff;

	const int n_test_vectors;
	std::vector<B> test_vect;
	std::vector<R> metrics;

	const int n_competitors;
	std::vector<info> competitors; // map of the competitor metric and its related test vector position

	std::vector<R> Y_N_cha_i;

	std::vector<std::vector<bool>> tv_candidates;

	const std::vector<float>   Alpha;
public:

	Decoder_chase_pyndiah(const int& n_ite,
	                      const Interleaver<R> &pi,
	                      Decoder_HIHO<B> &hiho_r,
	                      Decoder_HIHO<B> &hiho_c,
	                      const std::vector<uint32_t> &info_bits_pos_r,
	                      const std::vector<uint32_t> &info_bits_pos_c,
	                      const R   alpha = 0.5,
	                      const int n_least_reliable_positions = 2,
	                      const int n_test_vectors = 0,
	                      const int n_competitors  = 0);

protected:
	void _decode(const R *Y_N, int return_K_siso); // return_K_siso = 0 then hard decode and fill V_K_i else if = 1 then hard decode and fill V_H_i else soft decode and fill Y_N_i

	void _decode_row_siso(const R *R_cha, const R *R_prime, R *R_dec, Decoder_HIHO<B> &hiho, const int size); // size is length with parity bit if any
	void _decode_row_siho(const R *R_cha, const R *R_prime, B *R_dec, Decoder_HIHO<B> &hiho, const int size,
	                      const std::vector<uint32_t>& info_bits_pos, const bool return_K = true);

	bool _decode_chase          (const R *R_prime, Decoder_HIHO<B> &hiho,    const int size);
	void find_least_reliable_pos(const R* R_prime,                           const int size);
	void compute_test_vectors   (Decoder_HIHO<B> &hiho,                      const int size);
	void compute_metrics        (const R* R_prime,                           const int size);
	void compute_reliability    (const R* R_cha, const R* R_prime, R* R_dec, const int size);

	void bit_flipping(B* hard_vect, const int c);

	void generate_bit_flipping_candidates();
};

}
}

#endif	/* DECODER_CHASE_PYNDIAH_HPP_ */