/********************************************************************************
*
*  Author:
*     name:  
*     phone: 
*     email: 
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 1.4 $
*    $Author: sivaraman $
*    $Date: 2013-01-10 19:58:21 $
*    $State: Exp $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file ndrop_mfrsr_vap.h
 *  ndrop_mfrsr VAP Header File.
 */

#ifndef _NDROP_MFRSR_VAP_H_
#define _NDROP_MFRSR_VAP_H_

#include "dsproc3.h"

/** ndrop_mfrsr output datastream names and levels */
#define NDROPMFRSR_C1_DS_NAME "ndropmfrsr"
#define NDROPMFRSR_C1_DS_LEVEL "c1"

/** Standard value used for missing data. */
#define MISSING_VALUE -9999

/**
 *  User Data.
 */
typedef struct
{
    const char *proc_name;
    time_t       begin_date;   /**< begin time of the processing interval  */
    time_t       end_date;     /**< end time of the processing interval    */
    int          ds_id;         /**< ID of output datastream */


} NDROPData;

/*******************************************************************************
*  Prototypes
*/

/* Main VAP Functions */

void *init_process(void);
void  finish_process(void *user_data);

int post_retrieval_hook(
        void     *user_data,
        time_t    begin_time,
        time_t    end_time,
        CDSGroup *ret_data);


#define TEMP_VALID_MIN 183.15
#define TEMP_VALID_MAX 550.0 
#define PRES_VALID_MIN 1000
#define PRES_VALID_MAX 110000
#define NDROP_MAX 10000000000.0
#define KELVIN 260.
#define RHOL 1000. //kg/m^3
#define GAS_CONSTANT 287.    //gas constant (J/K/Kg)
#define GRAVITY 9.8
#define EPS 0.62188 //Molecular weight of water/Mol weight of dry air (unites)
#define ALV 2500000. //Latent heat of vaporization (J/kg)
#define CP 1005.     //Specific heat at constant pressure (J/K/kg)
#define QC_TRANS_BIT1 1 //consolidated QC bits
#define QC_TRANS_BIT8 128
#define QC_TRANS_BIT9 256
#define QC_TRANS_BAD 1
#define QC_TRANS_IND 2


//Global attributes set to null.  These are default values.
//Site specific values need to released to the database which will override the header.

float DELTA_K = 0.1;
float DELTA_BETA = 0.1;
float DELTA_C2 = 0.05;
float K = 0.74;
float DEFAULT_CBH = 1000.0;

/* Main VAP Functions */
int reset_trans_qc(const char*, const char*);
int qc_set2(int, int);
int qc_good(int);
float find_sonde_base_meas(float **, float, float *, int, int);
float find_max_ref(float *, float, float, int);
float calc_c2(float, float, float);
float calc_sat_vap_pres(float);
void create_quicklooks();

int process_data(
    void     *user_data,
    time_t    begin_date,
    time_t    end_date,
    CDSGroup *trans_data);

#endif /* _NDROP_MFRSR_VAP_H_ */
