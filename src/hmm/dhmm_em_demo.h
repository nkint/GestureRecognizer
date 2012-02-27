/*
 * dhmm_em_demo.h
 *
 *  Created on: Feb 10, 2012
 *      Author: alberto
 */

#ifndef DHMM_EM_DEMO_H_
#define DHMM_EM_DEMO_H_

#include <limits>
#include <cmath>
#include <cfloat>
#include <armadillo>

using namespace arma;

// UTILS /////////////////////////////////////////////////////////////////////////////

static mat get_cols(mat X, uvec c) {
	/*
	 * return only column in vector col.
	 * to simulate this:
	 * MATLAB code> a = [1 2 3;4 5 6;7 8 9]
	 * MATLAB code> a(:,[2 3])
	 * .. should return
	 * 				 2     3
     * 	   	   	     5     6
     *               8     9
	 */

	mat Y(X.n_rows, c.n_elem);
	uvec::iterator a = c.begin();
	uvec::iterator b = c.end();
	int l, m=0;
	for(uvec::iterator i=a; i!=b; ++i) {
		l = *i;
		Y.col(m) = X.col(l);
		m++;
	}
	return Y;
}

static mat log(mat A) {
	/*
	 * log function operates element-wise on matrix A
	 */
	mat X(A);
	mat::iterator a = X.begin();
	mat::iterator b = X.end();

	for(mat::iterator i=a; i!=b; ++i) {
		(*i) = log(*i);
	}
	return X;
}

static mat vectorize(mat A) {
	/*
	 * vectorise a matrix (ie. concatenate all the columns or rows)
	 * MATLAB code> M(:)
	 */
	mat B = mat(A);
	B.reshape(B.n_rows*B.n_cols, 1);
	return B;
}

static std::pair<mat,double> normaliseC(mat A) {
	/*
	 * Make the entries of matrix A array sum to 1 and return the normalization constant
	 * in a std::pair<normalized matrix, normalization constant>
	 */
	double z = accu(vectorize(A));
	double s = z + (z==0);		// Set any zeros to one before dividing
								// This is valid, since c=0 => all i. A(i)=0 => the answer should be 0/1=0
	mat M = A / s;
	return std::pair<mat,double>(M, z);
}

static mat normalise(mat A) {
	/*
	 * Make the entries of matrix A array sum to 1
	 */
	std::pair<mat,double> ret = normaliseC(A);
	return ret.first;
}

static mat mk_stochastic(mat T) {
	/*
	 * Ensure the argument is a stochastic matrix, i.e., the sum over the last dimension is 1.
	 * If T is a vector, it will sum to 1 (TODO).
	 * If T is a matrix, each row will sum to 1.
	 * If T is a 3D array, then sum_k T(i,j,k) = 1 for all i,j (TODO).
	 *
	 */
	mat Z = sum(T,1);
	mat S = Z + (Z==0);
	mat norm = repmat(S, 1, T.n_cols);
	T = T / norm;
	return T;
}

static mat multinomial_prob(mat data, mat obsmat) {
	/*
	 * Evaluate pdf of conditional multinomial
	 * Notation: Y = observation (O values), Q = conditioning variable (K values)
	 *
	 * Inputs:
	 *  data(t) = t'th observation - must be an integer in {1,2,...,K}: cannot be 0!
	 *  obsmat(i,o) = Pr(Y(t)=o | Q(t)=i)
	 *
	 * Output:
	 *  B(i,t) = Pr(y(t) | Q(t)=i)
	 *
	 */

	int Q = obsmat.n_rows;
	int T = data.n_elem;
	mat B = zeros(Q,T);

	for (int t = 0; t < T; ++t) {
		if (data(t)>obsmat.n_cols) {
			std::cout << "Index exceeds matrix dimension. data(t):" << data(t) <<" obsmat.n_cols:" << obsmat.n_cols << std::endl;
			//return B;
		}
		else {
			B.col(t) = obsmat.col(data(t)-1);
		}
	}
	return B;
}

// UTILS DHMM_EM ////////////////////////////////////////////////////////////////////

static bool em_converged(double loglik, double previous_loglik, double threshold) {

	/*
	 * Has EM converged?
	 *
	 * We have converged if the slope of the log-likelihood function falls below 'threshold',
	 * i.e., |f(t) - f(t-1)| / avg < threshold,
	 * where avg = (|f(t)| + |f(t-1)|)/2 and f(t) is log lik at iteration t.
	 * 'threshold' defaults to 1e-4.
	 *
	 * This stopping criterion is from Numerical Recipes in C p423
	 *
	 * If we are doing MAP estimation (using priors), the likelihood can decrase,
	 * even though the mode of the posterior is increasing.
	 *
	 */

	double converged = 0;
	double decrease = 0;

	if (loglik - previous_loglik < -1e-3) { // allow for a little imprecision
		cout<<"******likelihood decreased from "<<previous_loglik<<" to "<<loglik<<endl;
		decrease = 1;
		converged = 0;
		return converged;
	}
	double delta_loglik = abs(loglik - previous_loglik);
	double avg_loglik = (abs(loglik) + abs(previous_loglik) + DBL_EPSILON)/2;
	if ((delta_loglik / avg_loglik) < threshold)
		converged = 1;
	return converged;
}

static void _fwdback(mat init_state_distrib, mat _transmat, mat obslik,
		mat &alpha, mat &beta, mat& gamma, double &loglik, mat &xi_summed,
		cube &gamma2, cube &obslik2,
		bool fwd_only, bool compute_gamma2) {

	/*
	 * Compute the posterior probs. in an HMM using the forwards backwards algo.
	 *
	 * Notation:
	 *  Y(t) = observation, Q(t) = hidden state, M(t) = mixture variable (for MOG outputs)
	 *  A(t) = discrete input (action) (for POMDP models)
	 *
	 * INPUT:
	 *  init_state_distrib(i) = Pr(Q(1) = i)
	 *  transmat(i,j) = Pr(Q(t) = j | Q(t-1)=i)
	 *   or transmat{a}(i,j) = Pr(Q(t) = j | Q(t-1)=i, A(t-1)=a) if there are discrete inputs
	 *  obslik(i,t) = Pr(Y(t)| Q(t)=i)
	 *
	 */

	bool scaled = true;
	bool maximize = false;
	bool compute_xi = true;

	int Q = obslik.n_rows;
	int T = obslik.n_cols;

	mat mixmat;
	mat act;	// qui act è tutti zero, altrimenti potrebbe essere un input, TODO aggiungere &act negli input
	mat scale;

	if (obslik2.is_empty())
		compute_gamma2 = false;

	act = zeros(1,T);			// TODO this could be a colvec
	scale = ones(1,T);
	field<mat> transmat(1,1);
	transmat(0,0) = _transmat;

	// scale(t) = Pr(O(t) | O(1:t-1)) = gamma21/c(t) as defined by Rabiner (1989).
	// Hence prod_t scale(t) = Pr(O(1)) Pr(O(2)|O(1)) Pr(O(3) | O(1:2)) ... = Pr(O(1), ... ,O(T))
	// or log P = sum_t log scale(t).
	// Rabiner suggests multiplying beta(t) by scale(t), but we can instead
	// normalise beta(t) - the constants will cancel when we compute gamma.

	if (compute_xi)
		xi_summed = zeros(Q,Q);
	//else
	// xi_summed = [];

	//%%%%%%%%% Forwards %%%%%%%%%%
	//cout << "fwdback > Forwards" << endl;

	int t = 0;
	alpha.col(0) = vectorize(init_state_distrib) % obslik.col(t);
	if (scaled){
		std::pair<mat,double> _tmp = normaliseC(alpha.col(t));
		alpha.col(t) = _tmp.first;
		scale(t) = _tmp.second;
	}

	for(int t=1; t<T; t++) {
		mat trans;
		mat m;

		trans = transmat(act(t-1));

		if (maximize){
			//m = max_mult(trans.t(), alpha.col(t-1)); // TODO max_mult
		} else {
			m = trans.t() * alpha.col(t-1);
		}

		alpha.col(t) = vectorize(m) % obslik.col(t);

		if (scaled) {
			std::pair<mat,double> _tmp = normaliseC(alpha.col(t));
			alpha.col(t) = _tmp.first;
			scale(t) = _tmp.second;
		}

		if (compute_xi && fwd_only) {// useful for online EM
			xi_summed = xi_summed + normalise((alpha.col(t-1) * obslik.col(t).t()) % trans);
		}
	}

	if (scaled) {
		uvec _s = find(scale);  	// se c'è almeno uno zero
									// portando a logaritmo c'è almeno un infinito
									// quindi somma tutto a infinito
		if ( _s.is_empty() ) {
			loglik = -std::numeric_limits<double>::max();
		} else {
			loglik = sum(sum(log(scale))); // nested arma::sum because sum(mat X) return a rowvec
		}
	} else {
		loglik = log(sum(alpha.col(T)));
	}

	if (fwd_only) {
		gamma = alpha;
		return;
	}

	//%%%%%%%%% Backwards %%%%%%%%%%
	//cout << "fwdback > Backwards" << endl;

	int M;
	mat trans;
	mat denom;

	beta = zeros(Q,T);
	if (compute_gamma2) {
		M = mixmat.n_cols;
		gamma2 = zeros(Q,M,T);
	} else {
		//gamma2 = []
	}

	beta.col(T-1) = ones(Q,1);

	gamma.col(T-1) = normalise(alpha.col(T-1) % beta.col(T-1));
	t=T-1;

	if (compute_gamma2) {
		denom = obslik.col(t) + (obslik.col(t)==0); // replace 0s with 1s before dividing
		gamma2.slice(t) = obslik2.slice(t) % mixmat % repmat(gamma.col(t), 1, M) % repmat(denom, 1, M);
	}

	for (int t=T-2; t>=0; t--) { // T-2 because there are some calls to t+1
								 // and col(T) will generate the error Mat::col(): out of bounds
					             // so we must assure the limit of col(T-1)
		mat b = beta.col(t+1) % obslik.col(t+1);
		trans = transmat(act(t));
		if (maximize){
			mat B = repmat(vectorize(b).t(), Q, 1);
			beta.col(t) = max(trans % B, 1);
		} else
			beta.col(t) = trans * b;

		if (scaled)
			beta.col(t) = normalise( beta.col(t) );

		gamma.col(t) = normalise(alpha.col(t) % beta.col(t));

		if (compute_xi){
			xi_summed = xi_summed + normalise((trans % (alpha.col(t) * b.t())));
		}

		if (compute_gamma2){
			denom = obslik.col(t) + (obslik(t)==0); // replace 0s with 1s before dividing
			gamma2.slice(t) = obslik2.slice(t) % mixmat % repmat(gamma.col(t), 1, M) % repmat(denom, 1, M);
		}
	}
}

static void fwd(mat init_state_distrib, mat _transmat, mat obslik,
		mat &alpha, mat &beta, mat& gamma, double &loglik, mat &xi_summed, cube &gamma2, cube &obslik2) {

	_fwdback(init_state_distrib, _transmat, obslik,
			alpha, beta, gamma, loglik, xi_summed, gamma2, obslik2,
			true, false);
}

static void fwdback(mat init_state_distrib, mat _transmat, mat obslik,
		mat& gamma, double &loglik, mat &xi_summed) {
	/*
	 * caller of forward-backwards algorithm without:
	 *  - compute_gamma2
	 *  - returning of alpha and beta
	 */

	cube gamma2;
	cube obslik2;

	int Q = obslik.n_rows;
	int N = obslik.n_cols;

	mat alpha = zeros(Q,N);
	mat beta; 		// if fwd only beta is null, but in bwd is zeros(Q,T)

	_fwdback(init_state_distrib, _transmat, obslik,
			alpha, beta, gamma, loglik, xi_summed, gamma2, obslik2,
			false, false);

}

static void compute_ess_dhmm(mat startprob, mat transmat, mat obsmat, field<rowvec> data,
		double &loglik, mat &exp_num_trans, mat &exp_num_visits1, mat &exp_num_emit, mat &exp_num_visitsT) {
	/*
	 * Compute the Expected Sufficient Statistics for an HMM with discrete outputs
	 *
	 * INPUTS:
	 *  startprob(i)
	 *  transmat(i,j)
	 *  obsmat(i,o)
	 *  data{seq}(t)
	 *  dirichlet - weighting term for uniform dirichlet prior on expected emissions
	 *
	 * OUTPUTS:
	 *  exp_num_trans(i,j) = sum_l sum_{t=2}^T Pr(X(t-1) = i, X(t) = j| Obs(l))
	 *  exp_num_visits1(i) = sum_l Pr(X(1)=i | Obs(l))
	 *  exp_num_visitsT(i) = sum_l Pr(X(T)=i | Obs(l))
	 *  exp_num_emit(i,o) = sum_l sum_{t=1}^T Pr(X(t) = i, O(t)=o| Obs(l))
	 *  where Obs(l) = O_1 .. O_T for sequence l.
	 *
	 *
	 */
	//cout << "compute_ess_dhmm" << endl;

	int numex = data.n_elem;
	int O = obsmat.n_cols;

	for (int ex=0; ex<numex; ex++) {

		// initialization of EM
		rowvec obs = data(ex,0);

		mat obslik = multinomial_prob(obs, obsmat);

		int T = obs.n_elem;
		int Q = obslik.n_rows;
		int N = obslik.n_cols;
		double current_ll = 0;

		mat gamma = zeros(Q,N);
		mat xi_summed; 	// if compute_xi is zeros(Q,Q), else is null

		fwdback(startprob, transmat, obslik,
				gamma, current_ll, xi_summed);

		loglik = loglik +  current_ll;
		exp_num_trans = exp_num_trans + xi_summed;
		exp_num_visits1 = exp_num_visits1 + gamma.col(0);
		exp_num_visitsT = exp_num_visitsT + gamma.col(T-1);

		if (T < O) {
			for(int t=1; t<T; t++) {
				int o = obs[t];
				exp_num_emit.col(o-1) = exp_num_emit.col(o-1) + gamma.col(t);
			}
		} else {
			for(int o=0; o<=O; o++) {
				uvec ndx = find(obs==o);
				if (!ndx.is_empty()) {
					mat sumgamma = sum(get_cols(gamma,ndx), 1);
					exp_num_emit.col(o-1) = exp_num_emit.col(o-1) + sumgamma;
				}
			}
		}
	}
}

static vector<double> dhmm_em(field<rowvec> &data, mat &prior, mat &transmat, mat &obsmat, int max_iter) {
	/*
	 * Find the ML/MAP parameters of an HMM with discrete outputs using EM.
	 *
	 * Notation: Q(t) = hidden state, Y(t) = observation
	 */
	//cout << "dhmm_em"<<endl;

	double previous_loglik = - std::numeric_limits<double>::max();
	double thresh =  1e-4;
	double converged = 0;
	int num_iter = 1;
	int obs_prior_weight = 0; 	// weighting term for uniform dirichlet prior on expected emissions

	vector<double> LL;

	while ((num_iter <= max_iter) && !converged)  {

		double loglik = 0;
		int S = obsmat.n_rows;
		int O = obsmat.n_cols;

		mat exp_num_trans = zeros(S,S);		// all values initialized here for allocation issue (?) TODO: to be tested
		mat exp_num_visits1 = zeros(S,1);
		mat exp_num_visitsT = zeros(S,1);
		mat exp_num_emit = obs_prior_weight*ones(S,O);	// obs_prior_weight is dirichlet in Kevin Murphy's code

		// E step
		compute_ess_dhmm(prior, transmat, obsmat, data,
				loglik, exp_num_trans, exp_num_visits1, exp_num_emit, exp_num_visitsT);

		// M step
		prior = normalise(exp_num_visits1);
		if (!exp_num_trans.is_empty())
			transmat = mk_stochastic(exp_num_trans);
		obsmat = mk_stochastic(exp_num_emit);

		std::cout << "iteration " << num_iter << ", loglik = " << loglik << endl;

		num_iter =  num_iter + 1;
		converged = em_converged(loglik, previous_loglik, thresh);
		previous_loglik = loglik;
		LL.push_back(loglik);
	}
	return LL;
}

static double dhmm_logprob(field<rowvec> data, mat prior, mat transmat, mat obsmat) {
	/*
	 *  Compute the log-likelihood of a dataset using a discrete HMM
	 */

	int ncases = data.n_elem;
	double loglik = 0;

	for (int m=0; m<ncases; m++) {//m=1:ncases

		mat obslik = multinomial_prob(data(m,0), obsmat);

		int T = data.n_elem;
		int Q = obslik.n_rows;
		int N = obslik.n_cols;
		double ll = 0;

		mat alpha = zeros(Q,N);
		mat gamma = zeros(Q,N);
		mat beta; 		// if fwd only beta is null, but in bwd is zeros(Q,T)
		mat xi_summed; 	// if compute_xi is zeros(Q,Q), else is null

		cube gamma2;
		cube obslik2;

		fwd(prior, transmat, obslik,
			alpha, beta, gamma, ll, xi_summed, gamma2, obslik2);

	  if (ll == -std::numeric_limits<double>::max()) {
		  cout << "dhmm_logprob loglik is -inf!!" << endl;
	  }

	  loglik = loglik + ll;
	}

	return loglik;
}


static void demo() {
	cout << "Kevin Murphy demo" << endl;

	int O = 10; // alphabet
	int Q = 4; // hidden states

	// training data
	int T = 6;
	int nex = 7;

	mat _data;
	_data << 1 << 1 << 1 << 3 << 3 << 6 << endr <<
			1 << 1 << 3 << 3 << 6 << 6 << endr <<
			1 << 1 << 3 << 3 << 6 << 6 << endr <<
			1 << 1 << 3 << 3 << 3 << 6 << endr <<
			1 << 1 << 2 << 3 << 3 << 6 << endr <<
			1 << 1 << 3 << 3 << 6 << 6 << endr <<
			1 << 1 << 3 << 6 << 6 << 6 << endr;

	_data = mat("2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   2.0000   5.0000   5.0000   5.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   8.0000   7.0000   7.0000   7.0000   7.0000   7.0000   7.0000   7.0000   7.0000   7.0000   7.0000   7.0000   7.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000   6.0000");

	cout << "dudee" << endl;

	// num2cell snippet
	field<rowvec> data(_data.n_rows,1);
	for (uint r = 0; r < _data.n_rows; ++r) {
		data(r,0) = _data.row(r);
	}
	//data.print("data");

//	mat prior1 = normalise(ones(Q,1));
//	mat transmat1 = mk_stochastic(ones(Q,Q));
//	mat obsmat1 = mk_stochastic(ones(Q,O));
//	mat prior1 = normalise(randu<mat>(Q,1));
//	mat transmat1 = mk_stochastic(randu<mat>(Q,Q));
//	mat obsmat1 = mk_stochastic(randu<mat>(Q,O));

	mat prior1=mat("0.3078;0.1668;0.4191;0.1063");
	//cout << "dudee" << endl;

	mat transmat1=mat("0.2789    0.3169    0.1977    0.2065;0.1515    0.0670    0.3598    0.4217;0.1438    0.3627    0.1744    0.3191;0.2500    0.3100    0.1224    0.3176;");
	//cout << "dudee" << endl;

	mat obsmat1=mat("0.1374    0.0748    0.1328    0.1005    0.0482    0.0664    0.1930    0.0550 0.1286    0.0633; 0.0670    0.1663    0.1039    0.0408    0.0302    0.1635    0.1735    0.0724 0.1259    0.0564; 0.1599    0.1725    0.0409    0.1663    0.0448    0.0847    0.0864    0.1172 0.0437    0.0835; 0.1666    0.1720    0.0942    0.0609    0.1362    0.0578    0.0347    0.0820 0.0367    0.1588;");
	//cout << "dudee" << endl;


	vector<double> LL = dhmm_em(data, prior1, transmat1, obsmat1, 15);

	//prior1.print("prior1");
	//transmat1.print("transmat1");
	//obsmat1.print("obsmat1");

	// 0.3571    0.0238    0.3333         0         0    0.2857         0         0         0         0

	mat _data1;
	_data1 << //1 << 3 << 6 << endr;// <<
	 		 5 << 2 << 2 << endr ; //<<
	 		 //1 << 3 << 6 << endr;
	// num2cell snippet
	field<rowvec> data1(_data1.n_rows,1);
	for (uint r = 0; r < _data1.n_rows; ++r) {
		data1(r,0) = _data1.row(r);
	}
	double loglik1 = dhmm_logprob(data1, prior1, transmat1, obsmat1);

	cout << loglik1 << endl;
}

static void demo1() {
	// chaincode data set for class '0'
	field<rowvec> data0(3,1);
	data0(0,0) = rowvec("9 8 8 7 7 7 6 6 6 5 5 5 4 4 3 2 1 16 15 15 15 15 14 14 14 13 13 13 12 12 11 10 9 9 8 ");
	data0(1,0) = rowvec("8 8 7 7 7 6 6 5 5 5 5 4 4 3 2 1 1 16 15 15 15 15 15 14 14 14 14 13 12 11 10 10 9 9 9 ");
	data0(2,0) = rowvec("7 6 6 6 6 6 6 5 5 5 4 3 2 1 16 16 16 15 15 15 15 14 14 14 14 14 14 13 11 10 9 9 8 8 8 8 7 7 6 6 ");

	// chaincode data set for class ‘1’
	field<rowvec> data1(3,1);
	data1(0,0) = rowvec("5 5 5 5 5 5 5 5 5 5 5 4 ");
	data1(1,0) = rowvec("5 6 6 6 6 6 6 6 6 5 5 4 ");
	data1(2,0) = rowvec("5 5 5 6 6 6 6 6 6 7 6 4 3");

	// HMM for class '0' and random initialization of parameters
	mat hmm0prior("1 0 0");

	mat hmm0transmat = randu<mat>(3,3);
	hmm0transmat(1,0) = 0;
	hmm0transmat(2,0) = 0;
	hmm0transmat(2,1) = 0;
	hmm0transmat = mk_stochastic(hmm0transmat);
	//hmm0transmat.print("hmm0 transmat");
	hmm0transmat = mat("0.1531    0.6998    0.1471; 0    0.4340    0.5660; 0         0    1.0000");

	mat hmm0obsmat = randu<mat>(3, 18); // # of states * # of observation
	hmm0obsmat = mk_stochastic(hmm0obsmat);
	//hmm0obsmat.print("hmm0 obsmat");
	hmm0obsmat = mat("0.1252    0.0118    0.0061    0.0895    0.0029    0.1010    0.0836    0.0373    0.0308    0.0041    0.1086    0.1272    0.1061    0.1217    0.0295    0.0145; 0.0254    0.0725    0.0819    0.0435    0.1032    0.0921    0.0652    0.0282    0.0912    0.0607    0.1121    0.0021    0.0605    0.0709    0.0206    0.0699; 0.0497    0.0240    0.0462    0.0834    0.1064    0.0509    0.0705    0.0600    0.1311    0.0116    0.0089    0.0909    0.1177    0.0183    0.0056    0.1249");

	// HMM for class '1' and random initialiation of parameters
	mat hmm1prior("1 0");

	mat hmm1transmat = randu<mat>(2,2); // 2 by 2 transition matrix
	hmm1transmat(1,0) = 0;
	hmm1transmat = mk_stochastic(hmm1transmat);
	//hmm1transmat.print("hmm1 transmat");
	hmm1transmat = mat("0.2648    0.7352; 0    1.0000");

	mat hmm1obsmat = randu<mat>(2, 18); // # of states * # of observation
	hmm1obsmat = mk_stochastic(hmm1obsmat);
	//hmm1obsmat.print("hmm1 obsmat");
	hmm1obsmat = mat("0.0705    0.0799    0.0690    0.0308    0.0550    0.0628    0.0608    0.0608    0.0086    0.1038    0.1012    0.0924    0.0431    0.0131    0.0757    0.0723; 0.1289    0.0439    0.0318    0.0887    0.0537    0.0979    0.0719    0.0667    0.0938    0.0462    0.0452    0.0593    0.0284    0.0403    0.1021    0.0013");

	// Training of HMM model 0 (Baum-Welch algorithm)
	//[LL0, hmm0.prior, hmm0.transmat, hmm0.obsmat] = dhmm_em(data0, hmm0.prior, hmm0.transmat, hmm0.obsmat)
	cout << "=========================================== learn 0" << endl;
	vector<double> LL0 = dhmm_em(data0, hmm0prior, hmm0transmat, hmm0obsmat, 10);

	//% smoothing of HMM observation parameter: set floor value 1.0e-5
	//%hmm0.obsmat = max(hmm0.obsmat, 1.0e-5);
	//% Training of HMM model 1 (Baum-Welch algorithm)
	//[LL1, hmm1.prior, hmm1.transmat, hmm1.obsmat] = dhmm_em(data1, hmm1.prior, hmm1.transmat, hmm1.obsmat)
	cout << "=========================================== learn 1" << endl;
	vector<double> LL1 = dhmm_em(data1, hmm1prior, hmm1transmat, hmm1obsmat, 10);
	//% smoothing of HMM observation parameter: set floor value 1.0e-5
	//%hmm1.obsmat = max(hmm1.obsmat, 1.0e-5);

	cout << "=========================================== evaluation " << endl;
	//% Compare model likelihood
	//%Evaluation of class '0' data
	for (int dt=0; dt<data0.n_elem; ++dt) {
		double loglike0 = dhmm_logprob(data0.row(dt), hmm0prior, hmm0transmat, hmm0obsmat);
		double loglike1 = dhmm_logprob(data0.row(dt), hmm1prior, hmm1transmat, hmm1obsmat);
		printf("[class 0: %d-th data] model 0: %3f, model 1: %3f\n",dt, loglike0, loglike1);
	}

	for (int dt=0; dt<data1.n_elem; ++dt) {
		double loglike0 = dhmm_logprob(data1.row(dt), hmm0prior, hmm0transmat, hmm0obsmat);
		double loglike1 = dhmm_logprob(data1.row(dt), hmm1prior, hmm1transmat, hmm1obsmat);
		printf("[class 1: %d-th data] model 0: %3f, model 1: %3f\n",dt, loglike0, loglike1);
	}

}



#endif /* DHMM_EM_DEMO_H_ */
