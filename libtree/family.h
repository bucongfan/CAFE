#ifndef __FAMILIY_H__
#define __FAMILIY_H__

#include<tree.h>
#include<birthdeath.h>
#include "input_values.h"

#define FAMILYSIZEMAX	1000

typedef struct {
    int min;
    int max;
    int root_min;
    int root_max;
} family_size_range;

typedef struct 
{
	Tree	super;
	family_size_range range;	
	double	lambda;
	double mu;
	int branch_params_cnt;
	int k;
	int		size_of_factor;
	int 	rfsize;
}CafeTree;
typedef CafeTree* pCafeTree;


typedef struct
{
    char* errorfilename;
    int fromdiff;
    int todiff;
    int maxfamilysize;
    double** errormatrix;
}ErrorStruct;
typedef ErrorStruct* pErrorStruct;       

struct probabilities
{
	double  lambda;
	double	mu;
	double* param_lambdas;
	double* param_mus;
};

void free_probabilities(struct probabilities *probs);

/** Struct that holds information about a node in a CafeTree. It extends the 
	PhylogenyNode structure which in turn extends the TreeNode structure.
*/
typedef struct
{
	PhylogenyNode super;
	double** k_likelihoods;
	double* likelihoods;
	int*    viterbi;
	
	/**
	* Value representing the size of a single gene family for the species or node
	* Temporary value since CAFE holds in memory counts for many gene families for
	* each node
	*/
	int	familysize;	


	struct probabilities birth_death_probabilities;

	/** Matrix of precalculated values, indexed by the root family size
		and the family size
	*/
	struct square_matrix* birthdeath_matrix;
	pArrayList k_bd;
    pErrorStruct errormodel;
}CafeNode;
typedef CafeNode*	pCafeNode;


/**
* \brief Structure representing a matrix of values of family sizes
*
* Species array contains a list of names of species. Each item in flist
* holds a pCafeFamilyItem which holds a description and family ID, and
* and an array of integer family sizes in the order of the species given
*/
typedef struct
{
	char** species;				///< Names (ID's) of the species loaded into the family
	int   num_species;			///< Total number of species loaded
	int*  index;				///< indices of the species into the matching \ref CafeTree that was loaded by the user
    pErrorStruct* error_ptr;    ///< array of ErrorStruct pointers in the same order as species. the pointers point to errors[]. 
	int   max_size;
	pArrayList flist;   ///< family sizes
    pArrayList errors;  ///< list of actual ErrorStruct instances
    int** countbackup;  ///< space to store the real counts while simulating error
}CafeFamily;
typedef CafeFamily* pCafeFamily;

void init_family_size(family_size_range* fs, int max);
void copy_range_to_tree(pCafeTree tree, family_size_range* range);

typedef struct tagCafeParam CafeParam;
typedef CafeParam* pCafeParam;
typedef void (*param_func)(pCafeParam param, double* parameters);
enum OPTIMIZER_INIT_TYPE { UNKNOWN, DO_NOTHING, LAMBDA_ONLY, LAMBDA_MU };
void cafe_shell_set_lambdas(pCafeParam param, double* lambda);

/**
* \brief Singleton structure that holds all of the global data that Cafe acts on.
*
* Initialized at program startup by \ref cafe_shell_init
*/
struct tagCafeParam
{
	FILE *fout, *flog;
	pString str_fdata;

	/// tree information stored when the user calls the "tree" command
	pCafeTree pcafe;		

	/// family information stored when the user calls the "load" command
	pCafeFamily pfamily;	
	
	int eqbg;
	int posterior;

	/// Max Likelihood - Initialized by the "load" command with the number of families in the table
	double* ML;

	/// root size condition with max likelihood for each family	- Initialized by the "load" command with the number of families in the table
	double* MAP;

	/// prior is a poisson distribution on the root size based on leaves' size
	double* prior_rfsize;

	input_values input;

    int num_params;

    enum OPTIMIZER_INIT_TYPE optimizer_init_type;

	double* lambda;
	pTree lambda_tree;
	int num_lambdas;

	double* mu;
	int num_mus;
	
	int parameterized_k_value;
	double* k_weights;
	double** p_z_membership;
	int fixcluster0;

	int checkconv;
    int* old_branchlength;

    int num_branches;

	family_size_range family_size;
	
	int* root_dist;

	double pvalue;
	
	int  num_threads;

	double** likelihoodRatios;

	int quiet;
};


/****************************************************************************
 * Cafe Main
****************************************************************************/

extern void thread_run_with_arraylist(int numthreads, void* (*run)(void*), pArrayList pal );
// cafe tree
extern pBirthDeathCacheArray cafe_tree_set_birthdeath(pCafeTree pcafe, int max_family_size);
void free_cache_keep_matrices(pBirthDeathCacheArray cache);
#endif
