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
*    $Revision: 1.7 $
*    $Author: sivaraman $
*    $Date: 2013/02/19 21:02:56 $
*    $State: Exp $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file ndrop_mfrsr_vap.c
 *  ndrop_mfrsr VAP Main Functions.
 */

#include "ndrop_mfrsr_vap.h"
#include "ndrop_mfrsr_trans_fields.h"
#include "ndrop_mfrsr_output_fields.h"
#include <math.h>

static char *gVersion = "$State: Exp $";
char begin_date_str[24];
char end_date_str[24];
const char *site;
const char *facility;




/**
 *  @defgroup ndrop_mfrsr_VAP ndrop_mfrsr VAP
 */

/*@{*/

/**
 *  Initialize the process.
 *
 *  This function is used to do any up front process initialization that
 *  is specific to this process, and to create the UserData structure that
 *  will be passed to all hook functions.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @return
 *    - void pointer to the UserData structure
 *    - NULL if an error occurred
 */
void *init_process(void)
{
    NDROPData *data = (NDROPData *)NULL;

    DSPROC_DEBUG_LV1(
        "Creating user defined data structure\n");

    data = (NDROPData *)calloc(1, sizeof(NDROPData));

    if (!data) {

        DSPROC_ERROR(
            DSPROC_ENOMEM,
            "Memory allocation error creating user data structure\n");

        return((void *)NULL);
    }

    data->proc_name = dsproc_get_name();
    dsproc_set_trans_qc_rollup_flag(1);


    return((void *)data);
}

/**
 *  Finish the process.
 *
 *  This function frees all memory used by the UserData structure.
 *
 *  @param user_data  - void pointer to the UserData structure
 *                      returned by the init_process() function
 */
void finish_process(void *user_data)
{
    NDROPData *data = (NDROPData *)user_data;

    DSPROC_DEBUG_LV1(
        "Cleaning up user defined data structure for process\n");

    if (data) {
        free(data);
        
    }
}

/**
 *  Hook function called just prior to data transformation.
 *
 *  This function will be called once per processing interval just prior to
 *  data transformation, and after the retrieved observations are merged.
 *
 *  @param user_data  - void pointer to the UserData structure
 *                      returned by the init_process() function
 *  @param begin_date - the begin time of the current processing interval
 *  @param end_date   - the end time of the current processing interval
 *  @param ret_data   - pointer to the parent CDSGroup containing all the
 *                      retrieved data
 *
 *  @return
 *    -  1 if processing should continue normally
 *    -  0 if processing should skip the current processing interval
 *         and continue on to the next one.
 *    - -1 if a fatal error occurred and the process should exit.
 */
int pre_transform_hook(
    void     *user_data,
    time_t    begin_date,
    time_t    end_date,
    CDSGroup *ret_data)
{
    NDROPData *data = (NDROPData *)user_data;
    int       status    = 1;

/* Sample call to user pre_transform hook function */
/*    status = ndrop_mfrsr_pre_transform_hook( 
            user_data, begin_date, end_date, ret_data); */

    return(status);
}
/**
 *  *  Hook function called just after data is retrieved.
 *   *
 *    *  This function will be called once per processing interval just after
 *     *  data retrieval, but before the retrieved observations are merged and
 *      *  QC is applied.
 *       *
 *        *  @param user_data  - void pointer to the UserData structure
 *         *  @param begin_date - the begin time of the current processing interval
 *          *  @param end_date   - the end time of the current processing interval
 *           *  @param ret_data   - pointer to the parent CDSGroup containing all the
 *            *                      retrieved data
 *             *
 *              *  @return
 *               *    -  1 if processing should continue normally
 *                *    -  0 if processing should skip the current processing interval
 *                 *         and continue on to the next one.
 *                  *    - -1 if a fatal error occurred and the process should exit.
 *                   */
int post_retrieval_hook(
    void     *user_data,
    time_t    begin_date,
    time_t    end_date,
    CDSGroup *ret_data)
{
    CDSVar *ims_ht_var;
    char km_unit_value[64];
    char km_unit[64];
    size_t generic_length = 64;
    int status, obs_index;

    if (dsproc_get_debug_level() > 1) {

        dsproc_dump_retrieved_datasets(
            "./debug_dumps", "post_retrieval.debug", 0);
    }

    /* Coding for multiple observations but mergesonde should not have one*/

    for (obs_index = 0; ; obs_index++) {

      ims_ht_var = dsproc_get_retrieved_var("height", obs_index);

      if (!ims_ht_var) break;    
      dsproc_get_att_value(ims_ht_var,"units",
          CDS_CHAR, &generic_length, &km_unit_value);

      if (strcmp(km_unit_value, "km above MSL") == 0){
          DSPROC_DEBUG_LV1("****Found the unit for height %s\n", km_unit_value);
          sprintf(km_unit, "km");

          status = dsproc_set_att_value(ims_ht_var, "units", CDS_CHAR, strlen(km_unit), km_unit);
          if(status == 0) return(-1);

          dsproc_get_att_value(ims_ht_var,"units",
             CDS_CHAR, &generic_length, &km_unit_value);
          DSPROC_DEBUG_LV1("****Set the unit for height %s\n", km_unit);
      }
    }

    return(1);
}


/**
 *  Hook function called just after data transformation.
 *
 *  This function will be called once per processing interval just after data
 *  transformation, but before the process_data function is called.
 *
 *  @param  user_data - void pointer to the UserData structure
 *                      returned by the init_process() function
 *  @param begin_date - the begin time of the current processing interval
 *  @param end_date   - the end time of the current processing interval
 *  @param trans_data - pointer to the parent CDSGroup containing all the
 *                      transformed data
 *
 *  @return
 *    -  1 if processing should continue normally
 *    -  0 if processing should skip the current processing interval
 *         and continue on to the next one.
 *    - -1 if a fatal error occurred and the process should exit.
 */
int post_transform_hook(
    void     *user_data,
    time_t    begin_date,
    time_t    end_date,
    CDSGroup *trans_data)
{
    NDROPData *data = (NDROPData *)user_data;
    int       status = 1;

/*    status = ndrop_mfrsr_post_transform_hook( 
            user_data, begin_date, end_date, trans_data); */

    return(status);
}

/**
 *  Main data processing function.
 *
 *  This function will be called once per processing interval just after the
 *  output datasets are created, but before they are stored to disk.
 *
 *  @param  user_data - void pointer to the UserData structure
 *                      returned by the init_process() function
 *  @param  begin_date - begin time of the processing interval
 *  @param  end_date   - end time of the processing interval
 *  @param  trans_data - retriever data transformed to user defined 
                         coordinate systems
 *
 *  @return
 *    -  1 if processing should continue normally
 *    -  0 if processing should skip the current processing interval
 *         and continue on to the next one.
 *    - -1 if a fatal error occurred and the process should exit.

 */
int process_data(
    void     *user_data,
    time_t    begin_date,
    time_t    end_date,
    CDSGroup *trans_data)
{
    NDROPData *data = (NDROPData *)user_data;
    CDSGroup  *out_cds;
    int       nvars;
    CDSVar    **vars, **qc_vars;

    CDSVar    *t_cbh_var_arscl_be, *t_qc_cbh_var_arscl;
    CDSVar    *t_cbh_var_arscl_bot, *t_cbh_var_arscl_top;
    CDSVar    *t_opt_depth_mfrsr;
    CDSVar    *t_second_cbh_var_vceil, *t_third_cbh_var_vceil;
    CDSVar    *t_first_cbh_var_vceil, *t_qc_cbh_var_vceil;
    CDSVar    *t_tau_tot_error;
    CDSVar    *ocbh_source_ret_var_name, *ocbh_output,*ocbh_source_ret_var_name2, *iarscl_top;
    char      *ocbh_source_ds_name, *ocbh_source_var_name;
    char      *ocbh_source_ds_name2, *ocbh_source_var_name2;
    float     *omax_ref;
    int       nsamples, samp, ntsamples;
    float     *ocbh, *octh, *octhick, *ocb_temp, *ocb_pres, *obeta, *olwp_ad;
    float     *odnc, *odnc_ad, *otau, *obe_lwp, *obe_cbh_arscl, *oht_sonde, *osatwvp ;
    float     *odnc_tot_error, *otau_tot_error;
    float     delta_tau=0;
    float     delta_lwp =0;
    float     *ofirst_cbh, *osecond_cbh, *othird_cbh;
    float     **obar_pres, **otemp, **ocbh_arscl, **octh_arscl; 
    float     *oc2;
    float     c0 = 2.585;
    int       *oqc_cb_pres, *oqc_beta, *oqc_dnc, *oqc_dnc_ad, *oqc_lwp_ad;  
    int       *oqc_cb_temp, *octype, *oqc_cbh, *oqc_cth;
    int       *ocbh_source, *oqc_dnc_tot_error;
    int       nht, i, j, ngood_layers, nlayers, nliq_layers;
    int       *t_qc_cbh_arscl, *t_qc_cbh_vceil;
    
    static int QC_BIT_1=1; //tau = 0; 
    static int QC_BIT_2=2; //lwp = 0
    
    static int QC_NO_CTH = 3; //No cloud top available from arscl
    static int QC_CB_ICE = 4; //Lowest cloud layer base < 260
    static int QC_USE_DEFAULT =5;
    static int QC_CB_TEMP_PRES_MIN = 6; //QC for cloud base/pres failed valid_min
    static int QC_CB_TEMP_PRES_MAX = 7; //QC for cb pres and temp failed valid max
    static int QC_INDETERM_CBH = 8;     //QC for retrieval variables based on qc for cbh
    static int QC_INDETERM_NDROP_MAX = 9;     //QC for ndrop and ndrop_ad greater than 10^10 
    static int QC_BAD_NO_RETREIVAL =9;  //Used for beta, lwp and lwp_ad
    static int QC_INDETER_BETA_ZERO = 10; //Used for beta, lwp, lwp_ad and ndrop
    static int QC_CBH_3 = 3; //input transformation variables are indeterminate
    static int QC_CBH_4 = 4; //Used default cbh 
    static int QC_BAD_NDROP_TOTERR_MISSING =3; //ndrop_toterror 
    static int QC_BAD_NDROP_TAUERR =4; //ndrop_toterror 
    static int ARSCL_CBH=1;
    static int VCEIL_CBH=2;
    static int DEFAULT_CBH_SOURCE =3;

    static int LIQUID=1;
    static int ICE=2;
    static int MULTI=3;
    int status;
    int barscl = 1;
    int bvceil = 1;
    char source_att_value[64];
    char source_att_value2[64];
    char rundate[24], rundate2[24], sdd[24];
    float  max_sonde_ht, base_temp    ;
    size_t generic_length = 1;
    char *year, *month, *day, *ss;


    data->begin_date = begin_date;
    data->end_date   = end_date;

    format_secs1970(begin_date,rundate); 
    year = strtok (rundate,"-");
    month = strtok (NULL,"-");
    day = strtok (NULL,"-");
    memset(sdd, '\0', sizeof(sdd));
    strncpy (sdd, day,3);
    ss = strtok (NULL,":");
    strcpy(rundate2, year);
    strcat(rundate2, month);
    strcat(rundate2,sdd);


    site = dsproc_get_site();
    facility = dsproc_get_facility();
    ntsamples = 0;
    t_opt_depth_mfrsr = dsproc_get_transformed_var("optical_depth_instantaneous", 0);
    ntsamples = t_opt_depth_mfrsr->sample_count;
    DSPROC_DEBUG_LV1("The number of transformed samples %d \n", ntsamples);
    t_cbh_var_arscl_be = dsproc_get_transformed_var(transvarNameList_mfrsr_time_select_4[MFRSR_TIME_SELECT_4_CLOUDBASEBESTESTIMATE], 0);
    t_cbh_var_arscl_bot = dsproc_get_transformed_var(transvarNameList_mfrsr_time_select_4[MFRSR_TIME_SELECT_4_CLOUDLAYERBOTTOMHEIGHTMPLZWANG], 0);
    t_cbh_var_arscl_top = dsproc_get_transformed_var(transvarNameList_mfrsr_time_select_4[MFRSR_TIME_SELECT_4_CLOUDLAYERTOPHEIGHTMPLZWANG], 0);
    t_qc_cbh_var_arscl = dsproc_get_qc_var(t_cbh_var_arscl_be);
    t_tau_tot_error = dsproc_get_transformed_var(transvarNameList_mfrsr_time_mfrsrclod_vars[MFRSR_TIME_MFRSRCLOD_VARS_CLDTAUI_TOTERROR], 0); 
    otau_tot_error = (float *)  dsproc_alloc_var_data_index(t_tau_tot_error , 0, ntsamples);
    if (t_cbh_var_arscl_be){
       obe_cbh_arscl =  dsproc_alloc_var_data(t_cbh_var_arscl_be , 0, ntsamples);
       t_qc_cbh_arscl = dsproc_alloc_var_data(t_qc_cbh_var_arscl , 0, ntsamples);
       ocbh_arscl = (float **)  dsproc_alloc_var_data_index(t_cbh_var_arscl_bot , 0, ntsamples);
       octh_arscl = (float **)  dsproc_alloc_var_data_index(t_cbh_var_arscl_top , 0, ntsamples);
    } else {
       barscl = 0;
       obe_cbh_arscl =  (float *) malloc(ntsamples * sizeof(float));
       t_qc_cbh_arscl =  (int *) malloc(ntsamples * sizeof(float));
       ocbh_arscl =  (float *) malloc(ntsamples * sizeof(float));
       octh_arscl =  (float *) malloc(ntsamples * sizeof(float));
       DSPROC_DEBUG_LV1("There are no arscl input data so initializing the vars");

    }

    t_first_cbh_var_vceil = dsproc_get_transformed_var("first_cbh", 0);

    if (t_first_cbh_var_vceil){
       t_second_cbh_var_vceil = dsproc_get_transformed_var("second_cbh", 0);
       t_third_cbh_var_vceil = dsproc_get_transformed_var("third_cbh", 0);
       t_qc_cbh_var_vceil = dsproc_get_qc_var(t_first_cbh_var_vceil);
       ofirst_cbh = dsproc_alloc_var_data(t_first_cbh_var_vceil , 0, ntsamples);
       osecond_cbh = dsproc_alloc_var_data(t_second_cbh_var_vceil , 0, ntsamples);
       othird_cbh = dsproc_alloc_var_data(t_third_cbh_var_vceil , 0, ntsamples);
       t_qc_cbh_vceil = dsproc_alloc_var_data(t_qc_cbh_var_vceil, 0, ntsamples);
    } else {
      bvceil = 0;
      ofirst_cbh = (float *) malloc(ntsamples * sizeof(float));
      osecond_cbh = (float *) malloc(ntsamples * sizeof(float));
      othird_cbh = (float *) malloc(ntsamples * sizeof(float));
      t_qc_cbh_vceil = (int *) malloc(ntsamples * sizeof(float));
    }
    
    DSPROC_DEBUG_LV1("The number of samples in transformed is %d\n", ntsamples);
    
    
    /* ------------------------------------------------------------- */
    /* Start algorithm */
    /* ------------------------------------------------------------- */
    out_cds = dsproc_get_output_dataset(data->ds_id,0);
    if (!out_cds){
       return(-1);
    } 
    /* If any of these global attribute names are changed, then we need to fix the dsatts 
    file in /home/sivaraman/dev/vap/conf/vap/ndrop_mfrsr
    */
     if (!dsproc_set_att_value_if_null(out_cds,
          "cloud_base_height_default", CDS_FLOAT,1, &DEFAULT_CBH)) {
            return(-1);
    }
    if (!dsproc_set_att_value_if_null(out_cds,
          "delta_beta", CDS_FLOAT,1, &DELTA_BETA)) {
            return(-1);
    }
    if (!dsproc_set_att_value_if_null(out_cds,
          "delta_k", CDS_FLOAT,1, &DELTA_K)) {
            return(-1);
    }
    if (!dsproc_set_att_value_if_null(out_cds,
          "delta_c2", CDS_FLOAT,1, &DELTA_C2)) {
            return(-1);
    }
    

    dsproc_get_att_value(out_cds,"cloud_base_height_default", 
        CDS_FLOAT, &generic_length, &DEFAULT_CBH);
    dsproc_get_att_value(out_cds,"delta_beta", 
        CDS_FLOAT, &generic_length, &DELTA_BETA);
    dsproc_get_att_value(out_cds,"delta_k", 
        CDS_FLOAT, &generic_length, &DELTA_K);
    dsproc_get_att_value(out_cds,"delta_c2", 
        CDS_FLOAT, &generic_length, &DELTA_C2);

   if (barscl) { 
     if(!(ocbh_source_ret_var_name = dsproc_get_retrieved_var("CloudBaseBestEstimate", 0))) return (-1); //as defined in the PCM
     if(!(ocbh_source_ds_name      = dsproc_get_source_ds_name(ocbh_source_ret_var_name))) return(-1);
     if(!(ocbh_source_var_name     = dsproc_get_source_var_name(ocbh_source_ret_var_name))) return(-1); //this could be different from source name
   
     sprintf(source_att_value, ocbh_source_ds_name);
     strcat(source_att_value, ":");
     strcat(source_att_value, ocbh_source_var_name);
   } else {
     sprintf(source_att_value, "Not used");
   }
   if (bvceil){
     if(!(ocbh_source_ret_var_name2 = dsproc_get_retrieved_var("first_cbh", 0))) return (-1); //as defined in the PCM
     if(!(ocbh_source_ds_name2      = dsproc_get_source_ds_name(ocbh_source_ret_var_name2))) return(-1);
     if(!(ocbh_source_var_name2     = dsproc_get_source_var_name(ocbh_source_ret_var_name2))) return(-1); //this could be different from source name

     sprintf(source_att_value2, ocbh_source_ds_name2);
     strcat(source_att_value2, ":");
     strcat(source_att_value2, ocbh_source_var_name2);
    } else {
     sprintf(source_att_value2, "Not used");
   }

    if (dsproc_get_debug_level() > 1) {
          dsproc_dump_output_datasets(
            "./debug_dumps", "pre_process_data_output.debug", 0);
    }

    nvars = dsproc_get_dataset_vars(out_cds, outvarNameList_ndropmfrsr_c1, 1, &vars, &qc_vars, NULL);
    DSPROC_DEBUG_LV1("The number of vars are %d\n", nvars);
    
    if (nvars < 0) {
       return(-1);
    }
  
    ocbh_output = dsproc_get_output_var(data->ds_id, "source_cloud_base", 0);
    CDSAtt *flag1, *flag2;
    if(!(flag1 = dsproc_get_att(ocbh_output, "flag_1_description"))) return(-1);
    if(!(flag2 = dsproc_get_att(ocbh_output, "flag_2_description"))) return(-1);

    status = dsproc_set_att_value(ocbh_output, flag1->name, CDS_CHAR, strlen(source_att_value), source_att_value);
    if(status == 0) return(-1);
    status = dsproc_set_att_value(ocbh_output, flag2->name, CDS_CHAR, strlen(source_att_value2), source_att_value2);
    if(status == 0) return(-1);

    nsamples = vars[NDROPMFRSR_C1_OPTICAL_DEPTH_INSTANTANEOUS]->sample_count;
    DSPROC_DEBUG_LV1 ("The number of samples in output is %d\n", nsamples);
    nht      = vars[NDROPMFRSR_C1_HEIGHT]->sample_count;
    if (barscl) {
      if(!(iarscl_top = dsproc_get_retrieved_var("CloudLayerTopHeightMplZwang", 0))) return (-1);
      nlayers = iarscl_top->dims[1]->length;
    } else {
      nlayers  = 10; 
    }
    DSPROC_DEBUG_LV1("The number of layers is %d\n", nlayers);
    ocbh = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_CLOUD_BASE_HEIGHT] , 0, nsamples);
    octh = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_CLOUD_TOP_HEIGHT] , 0, nsamples);
    oqc_cbh = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_CLOUD_BASE_HEIGHT] , 0, nsamples);
    oqc_cth = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_CLOUD_TOP_HEIGHT] , 0, nsamples);
    octhick = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_CLOUD_THICKNESS] , 0, nsamples);
    octype = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_CLOUD_BASE_TYPE] , 0, nsamples);
    osatwvp = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_SATURATED_WATER_VAPOR_PRESSURE] , 0, nsamples);
    oc2 = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_CONDENSATION_RATE] , 0, nsamples);
    omax_ref= (float *) malloc(nsamples * sizeof(float));


  
    ocb_temp = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_CLOUD_BASE_TEMPERATURE] , 0, nsamples);
    ocb_pres = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_CLOUD_BASE_PRESSURE ] , 0, nsamples);
    oqc_cb_pres = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_CLOUD_BASE_PRESSURE ] , 0, nsamples);
    oqc_cb_temp  = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_CLOUD_BASE_TEMPERATURE] , 0, nsamples);
    obeta = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_BETA] , 0, nsamples);
    oqc_beta = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_BETA] , 0, nsamples);
    odnc  = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_DROP_NUMBER_CONC] , 0, nsamples);
    oqc_dnc  = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_DROP_NUMBER_CONC] , 0, nsamples);
    odnc_tot_error  = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_DROP_NUMBER_CONC_TOTERROR] , 0, nsamples);
    oqc_dnc_tot_error = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_DROP_NUMBER_CONC_TOTERROR] , 0, nsamples);
    odnc_ad  = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_DROP_NUMBER_CONC_ADIABATIC] , 0, nsamples);
    oqc_dnc_ad  = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_DROP_NUMBER_CONC_ADIABATIC] , 0, nsamples);
    olwp_ad  = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_LWP_ADIABATIC] , 0, nsamples);
    oqc_lwp_ad  = dsproc_alloc_var_data(qc_vars[NDROPMFRSR_C1_LWP_ADIABATIC] , 0, nsamples);
  
    ocbh_source = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_SOURCE_CLOUD_BASE] , 0, nsamples);
    otau = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_OPTICAL_DEPTH_INSTANTANEOUS] , 0, nsamples);
    obe_lwp = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_LWP_MEAS] , 0, nsamples);
    //Getting two dimensional data 
    obar_pres = (float **) dsproc_alloc_var_data_index(vars[NDROPMFRSR_C1_BAR_PRES] , 0, nsamples);
    otemp  = (float **) dsproc_alloc_var_data_index(vars[NDROPMFRSR_C1_TEMP] , 0, nsamples);
   
    oht_sonde = dsproc_alloc_var_data(vars[NDROPMFRSR_C1_HEIGHT] , 0, nht);
    for (samp = 0; samp < nsamples; samp++){
        oqc_cb_pres[samp] = 0;
        oqc_cb_temp[samp] = 0;
        oqc_beta[samp]    = 0;
        oqc_dnc[samp]     = 0;
        oqc_dnc_ad[samp]  = 0;
        oqc_lwp_ad[samp] = 0;
        //oqc_lwp[samp]     = 0;
        ocbh_source[samp] = -1;
        octype[samp]      = -1;
        oqc_cbh[samp]     = 0;
        oqc_cth[samp]     = 0;
        oqc_dnc_tot_error[samp] = 0;
       
        ocbh[samp] = MISSING_VALUE;
        octh[samp] = MISSING_VALUE;
        octhick[samp] = MISSING_VALUE;
        omax_ref[samp] = MISSING_VALUE;
        ocb_temp[samp] = MISSING_VALUE;
        ocb_pres[samp] = MISSING_VALUE;
        obeta[samp] = MISSING_VALUE;
        odnc[samp] = MISSING_VALUE;
        odnc_ad[samp] = MISSING_VALUE;
        olwp_ad[samp] = MISSING_VALUE;
        osatwvp[samp] = MISSING_VALUE;
        oc2[samp]     = MISSING_VALUE;
        odnc_tot_error[samp] = MISSING_VALUE;
        if (otau[samp] <= 0){
            oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_BIT_1);
            oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_BIT_1);
            oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_BIT_1);
            oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_BIT_1);
            //oqc_lwp[samp]    = qc_set2(oqc_lwp[samp], QC_BIT_1); 
            
            oqc_cbh[samp]  = qc_set2(oqc_cbh[samp], QC_BIT_1);
            oqc_cth[samp]  = qc_set2(oqc_cth[samp], QC_BIT_1);
            oqc_cb_temp[samp] = qc_set2(oqc_cb_temp[samp], QC_BIT_1); 
            oqc_cb_pres[samp] = qc_set2(oqc_cb_pres[samp], QC_BIT_1);
            oqc_dnc_tot_error[samp] = qc_set2(oqc_dnc_tot_error[samp], QC_BIT_1);
            ocbh_source[samp] = -1; //No source    
        }
         if (obe_lwp[samp] <= 0.02){
          
            oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_BIT_2);
            oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_BIT_2);
            oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_BIT_2);
            oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_BIT_2);
            //oqc_lwp[samp]    = qc_set2(oqc_lwp[samp], QC_BIT_2); 
            oqc_cbh[samp]  = qc_set2(oqc_cbh[samp], QC_BIT_2);
            oqc_cth[samp]  = qc_set2(oqc_cth[samp], QC_BIT_2); 
            oqc_cb_temp[samp] = qc_set2(oqc_cb_temp[samp], QC_BIT_2);
            oqc_cb_pres[samp] = qc_set2(oqc_cb_pres[samp], QC_BIT_2);
            oqc_dnc_tot_error[samp] = qc_set2(oqc_dnc_tot_error[samp], QC_BIT_2);
            ocbh_source[samp] = -1; //No source   
        }
    }
   
    max_sonde_ht = oht_sonde[nht-1];
    for (samp = 0; samp < nsamples; samp++){
    
      if ((otau[samp] > 0) && (obe_lwp[samp] > 0.02)){
      
        if ((barscl) && (obe_cbh_arscl[samp] > 0)){
            ocbh[samp] = obe_cbh_arscl[samp];
            
             /* Indeterminate qc for cbh is set because bad ones should be -9999*/
            if (!qc_good(t_qc_cbh_arscl[samp])){
                oqc_cbh[samp] = qc_set2(oqc_cbh[samp],QC_CBH_3);
                oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_INDETERM_CBH);
                oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_INDETERM_CBH);
                oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_INDETERM_CBH);
                oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_INDETERM_CBH);
            }
           
            ocbh_source[samp] = ARSCL_CBH; //ARSCL_CLOUD_BASE 
            
            i=1; 
            //Determine cloud base temperature and pressure from sonde
            ocb_pres[samp] = find_sonde_base_meas(obar_pres, ocbh[samp], oht_sonde, nht, samp);
            ocb_temp[samp] = find_sonde_base_meas(otemp, ocbh[samp], oht_sonde, nht, samp);
          
            ngood_layers=0;
            if (ocb_temp[samp] < KELVIN){
                octype[samp] = ICE;
                obeta[samp] = MISSING_VALUE;
                odnc[samp] = MISSING_VALUE;
                odnc_ad[samp] = MISSING_VALUE;
                olwp_ad[samp] = MISSING_VALUE;
                //olwp[samp] = MISSING_VALUE;
                oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_ICE);
                oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_ICE);
                oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_ICE);
                oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_ICE);
                //oqc_lwp[samp]    = qc_set2(oqc_lwp[samp], QC_CB_ICE); 
            
            }
            /* This has to be the best estimate cloud base and not ocbh_arscl*/
            if ((octh_arscl[samp][0] > 0) && (octh_arscl[samp][0]> ocbh[samp])){
                octh[samp] = octh_arscl[samp][0]; 
                octhick[samp] = octh_arscl[samp][0] - ocbh[samp];
                j=0;
                ngood_layers++;
                j++;
                while (j < nlayers){
                //Making sure that the transformation qc is good */
                    if ((ocbh_arscl[samp][j] > 0) && (octh_arscl[samp][j] > 0) && (t_qc_cbh_arscl[samp] == 0)){ //this is from the radar              
                        ngood_layers++; 
                    }
                j++;
                }
                
                j=0;
                nliq_layers=0;
                while (j < ngood_layers){
                    base_temp = find_sonde_base_meas(otemp, ocbh_arscl[samp][j], oht_sonde, nht, samp);
                    
                    if (base_temp > KELVIN){
                        nliq_layers++;
                        if (nliq_layers ==1){
                            octype[samp] = LIQUID; 
                            omax_ref[samp] = find_max_ref(oht_sonde, ocbh_arscl[samp][j], octh_arscl[samp][j], nht);
                        }
                        else if (nliq_layers > 1){
                            octype[samp] = MULTI; //multiple layers
                        }
                    }
                    else {// base_temp < KELVIN
                        if (base_temp > MISSING_VALUE){
                            octype[samp] = ICE;
                            obeta[samp] = MISSING_VALUE;
                            odnc[samp] = MISSING_VALUE;
                            odnc_ad[samp] = MISSING_VALUE;
                            olwp_ad[samp] = MISSING_VALUE;
                            //oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_ICE);
                            //oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_ICE);
                            //oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_ICE);
                            //oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_ICE);
                        }
                    
                    }
                    j++;
                   
                }
            }
            else {  //arscl cloud top is less than best estimate cloud base
                omax_ref[samp] = MISSING_VALUE;
                
                octh[samp] = MISSING_VALUE;
                octhick[samp] = MISSING_VALUE;
                //Need to set qc to bad for ocbh and octh
                oqc_cth[samp] = qc_set2(oqc_cth[samp], QC_NO_CTH); //No cloud top 
                obeta[samp] = MISSING_VALUE;
                oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_NO_CTH);
                olwp_ad[samp] = MISSING_VALUE;
                oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_NO_CTH);
                //Need to set qc to Indeterminate for beta
                if (ocb_temp[samp] < KELVIN && (ocb_temp[samp] != MISSING_VALUE)){
                    octype[samp] = ICE; //ice
                    obeta[samp] = MISSING_VALUE;
                    odnc[samp] = MISSING_VALUE;
                    odnc_ad[samp] = MISSING_VALUE;
                    olwp_ad[samp] = MISSING_VALUE;
                    
                    oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_ICE);
                    oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_ICE);
                    oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_ICE);
                    oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_ICE);
                    
                }
                else
                    octype[samp] = LIQUID; 
             }      
                /**/
            }
            //No ARSCL CBH
        else{ //if obe_cbh_arscl[samp] is less than zero meaning no cloud base or NO ARSCL
            
            omax_ref[samp] = MISSING_VALUE;
            octh[samp] = MISSING_VALUE;
            octhick[samp] = MISSING_VALUE;
            oqc_cth[samp] = qc_set2(oqc_cth[samp], QC_NO_CTH); //No cloud top 
            oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_NO_CTH);
            olwp_ad[samp] = MISSING_VALUE;
            oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_NO_CTH);
            //No ARSCL but we have VCEIL CBH
            ngood_layers = 0;
            nliq_layers=0;
                    
                    //Assumption that first_cbh will be always set before second_cbh
            if ((bvceil) && (ofirst_cbh[samp] > 0)){
                ocbh_source[samp] = VCEIL_CBH; //VCEIL
                ocbh[samp] = ofirst_cbh[samp];
                        
                         /* Indeterminate qc for cbh is set because bad ones should be -9999*/
                if (!qc_good(t_qc_cbh_vceil[samp])){
                            
                    oqc_cbh[samp] = qc_set2(oqc_cbh[samp], QC_CBH_3);
                    oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_INDETERM_CBH);
                    oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_INDETERM_CBH);
                    oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_INDETERM_CBH);
                    oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_INDETERM_CBH);
                            //oqc_lwp[samp]    = qc_set2(oqc_lwp[samp], QC_INDETERM_CBH); 
                }
                       
                         //Determine cloud base temperature and pressure from sonde
                ocb_pres[samp] = find_sonde_base_meas(obar_pres, ocbh[samp], oht_sonde, nht, samp);
                ocb_temp[samp] = find_sonde_base_meas(otemp, ofirst_cbh[samp], oht_sonde, nht, samp);
                ngood_layers++;
                if (ocb_temp[samp] > KELVIN){
                    nliq_layers++;
                         
                }
                if (osecond_cbh[samp] > 0){
                    ngood_layers++;
                    base_temp = find_sonde_base_meas(otemp, osecond_cbh[samp], oht_sonde, nht, samp);
                    if (base_temp > KELVIN){
                        nliq_layers++;
                    }
                }
                if (othird_cbh[samp] > 0){
                    ngood_layers++;
                    base_temp = find_sonde_base_meas(otemp, othird_cbh[samp], oht_sonde, nht, samp);
                    if (base_temp > KELVIN){
                        nliq_layers++;
                    }
                }
                        
                if (ngood_layers ==1){
                    octype[samp] = LIQUID;
                }
                if (ngood_layers > 1){
                    octype[samp] = MULTI; //multiple layers
                }
                if (ngood_layers ==0){
                    octype[samp] =ICE; //ice
                    obeta[samp] = MISSING_VALUE;
                    odnc[samp] = MISSING_VALUE;
                    odnc_ad[samp] = MISSING_VALUE;
                    olwp_ad[samp] = MISSING_VALUE;
                    //olwp[samp] = MISSING_VALUE;
                    oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_ICE);
                    oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_ICE);
                    oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_ICE);
                    oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_ICE);
                    //oqc_lwp[samp]    = qc_set2(oqc_lwp[samp], QC_CB_ICE); 
                }
            } // end of first_cbh
            else{ // No vceil cbh
                ocbh[samp] = DEFAULT_CBH;
                ocbh_source[samp] = DEFAULT_CBH_SOURCE; //DEFAULT CBH of 1.0

                
                oqc_cbh[samp] = qc_set2(oqc_cbh[samp], QC_CBH_4);
                //Indeterminate flags
                ocb_pres[samp] = find_sonde_base_meas(obar_pres, DEFAULT_CBH, oht_sonde, nht, samp);
                ocb_temp[samp] = find_sonde_base_meas(otemp, DEFAULT_CBH, oht_sonde, nht, samp);
                oqc_cb_temp[samp] = qc_set2(oqc_cb_temp[samp], QC_USE_DEFAULT); 
                oqc_cb_pres[samp] = qc_set2(oqc_cb_pres[samp], QC_USE_DEFAULT);
                if (ocb_temp[samp] > KELVIN){
                    octype[samp] =LIQUID;  //liquid
                }
                if ((ocb_temp[samp] > MISSING_VALUE) && (ocb_temp[samp] < KELVIN)){
                    octype[samp] =ICE;  //ice
                    oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_ICE);
                    oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_ICE);
                    oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_ICE);
                    oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_ICE);
                    //oqc_lwp[samp]    = qc_set2(oqc_lwp[samp], QC_CB_ICE); 
                            
                }
                    obeta[samp] = MISSING_VALUE;
                    odnc[samp] = MISSING_VALUE;
                    odnc_ad[samp] = MISSING_VALUE;
                    olwp_ad[samp] = MISSING_VALUE;
                        
                    oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_USE_DEFAULT);
                    oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_USE_DEFAULT);
                    oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_USE_DEFAULT);
                    oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_USE_DEFAULT);
                }
            } //end of else arscl be cbh
      } //tau < 0  
    } //for samp
   
   
    for (samp =0; samp< nsamples; samp++){
      if ((otau[samp] > 0) && (obe_lwp[samp] > 0.02)){
        if ((ocb_temp[samp] > MISSING_VALUE) && (ocb_temp[samp] < TEMP_VALID_MIN)) {
            oqc_cb_temp[samp] = qc_set2(oqc_cb_temp[samp], QC_CB_TEMP_PRES_MIN);
            //output variables
            obeta[samp] = MISSING_VALUE;
            odnc[samp] = MISSING_VALUE;
            odnc_ad[samp] = MISSING_VALUE;
            olwp_ad[samp] = MISSING_VALUE;
           
            oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_TEMP_PRES_MIN);
            oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_TEMP_PRES_MIN);
            oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_TEMP_PRES_MIN);
            oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_TEMP_PRES_MIN);
            
        }
        if ((ocb_pres[samp] > MISSING_VALUE) && (ocb_pres[samp] < PRES_VALID_MIN)) {
            oqc_cb_pres[samp] = qc_set2(oqc_cb_pres[samp], QC_CB_TEMP_PRES_MIN);
             //output variables
            obeta[samp] = MISSING_VALUE;
            odnc[samp] = MISSING_VALUE;
            odnc_ad[samp] = MISSING_VALUE;
            olwp_ad[samp] = MISSING_VALUE;
            
            oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_TEMP_PRES_MIN);
            oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_TEMP_PRES_MIN);
            oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_TEMP_PRES_MIN);
            oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_TEMP_PRES_MIN);
           
        }
        if (ocb_temp[samp] > TEMP_VALID_MAX){
            oqc_cb_temp[samp] = qc_set2(oqc_cb_temp[samp], QC_CB_TEMP_PRES_MAX);
             //output variables
            obeta[samp] = MISSING_VALUE;
            odnc[samp] = MISSING_VALUE;
            odnc_ad[samp] = MISSING_VALUE;
            olwp_ad[samp] = MISSING_VALUE;
            
            oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_TEMP_PRES_MAX);
            oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_TEMP_PRES_MAX);
            oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_TEMP_PRES_MAX);
            oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_TEMP_PRES_MAX);
            
        }
        if (ocb_pres[samp] > PRES_VALID_MAX){
            oqc_cb_pres[samp] = qc_set2(oqc_cb_pres[samp], QC_CB_TEMP_PRES_MAX);
             //output variables
            obeta[samp] = MISSING_VALUE;
            odnc[samp] = MISSING_VALUE;
            odnc_ad[samp] = MISSING_VALUE;
            olwp_ad[samp] = MISSING_VALUE;
          
            oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_TEMP_PRES_MAX);
            oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_CB_TEMP_PRES_MAX);
            oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_TEMP_PRES_MAX);
            oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_TEMP_PRES_MAX);
           
        }
      } //Valid tau and valid be_lwp
    
    }
    for (samp=0; samp<nsamples; samp++){
       //No retrieval can be done without tau and be_lwp and the qc will be set at the beginning
       if ((otau[samp] > 0) && (obe_lwp[samp] > 0.02)){
      
        // Decided to run without ARSCL May 2014
        //We need to make sure that the temperature is greater than 260 to calculate the osatwvp
        //since we do the retrieval only for liquid conditions. */
       
        if(ocb_temp[samp] > KELVIN){
            osatwvp[samp] = calc_sat_vap_pres(ocb_temp[samp]);
            //Need to check for valid points of sat water vapor pressure - ask Sally
        
            if ((ocbh_source[samp] == ARSCL_CBH) && 
                (octhick[samp] > 0) &&
                (ocb_pres[samp] > PRES_VALID_MIN &&
                ocb_pres[samp] < PRES_VALID_MAX)){
                    oc2[samp] = calc_c2(ocb_temp[samp], ocb_pres[samp], osatwvp[samp]); 

                    olwp_ad[samp] = 0.5 * oc2[samp] * (pow(octhick[samp],2));
                    obeta[samp] = 1.0 - (obe_lwp[samp]/olwp_ad[samp]);
                   
                    //Setting indeterminate qc when beta values are reset to zero
                    if (obeta[samp] < 0.0) {
                        obeta[samp] =0;
                        oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_INDETER_BETA_ZERO);
                        oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_INDETER_BETA_ZERO);
                        oqc_dnc[samp] = qc_set2(oqc_dnc[samp], QC_INDETER_BETA_ZERO);
                        
                    }
                    if (obeta[samp] > 1.0)
                        obeta[samp] =1;
            
            
                    odnc[samp] = 1./K * 
                                (pow((otau[samp]/c0),3)) * 
                                (pow(((1-obeta[samp]) * oc2[samp]), 0.5)) * 
                                (pow(RHOL,2)) / (pow(obe_lwp[samp],2.5));
                   
                    
                    //Here setting the beta to zero.
                    odnc_ad[samp] = 1./K * 
                                (pow((otau[samp]/c0),3)) * 
                                (pow((1-0) * oc2[samp], 0.5)) * 
                                (pow(RHOL,2))/(pow(obe_lwp[samp],2.5));
                    if (odnc[samp] > NDROP_MAX){
                       oqc_dnc[samp]  = qc_set2(oqc_dnc[samp], QC_INDETERM_NDROP_MAX);
                       
                    }
                    
                    if (odnc_ad[samp] > NDROP_MAX)
                       oqc_dnc_ad[samp]  = qc_set2(oqc_dnc_ad[samp], QC_INDETERM_NDROP_MAX);
                   

                    //Calculate Uncertainty estimates.
                    if (otau_tot_error[samp] > MISSING_VALUE){
                        delta_tau = otau_tot_error[samp]/otau[samp];
                        delta_lwp = .02/obe_lwp[samp];
                        odnc_tot_error[samp] = odnc[samp] * 
                                           sqrt(pow(DELTA_K,2) +
                                           pow((3.0*delta_tau),2) +
                                           pow((2.5*delta_lwp),2) +
                                           pow((0.5*DELTA_BETA),2) +
                                           pow((0.5*DELTA_C2),2));
                       
                    }
                    else
                        oqc_dnc_tot_error[samp] = qc_set2(oqc_dnc_tot_error[samp], QC_BAD_NDROP_TAUERR);
            }
            else { //Not ARSCL
                /*We are going to calculate dnc_lwp with beta being 0 */
                if ((ocb_pres[samp] > PRES_VALID_MIN &&
                ocb_pres[samp] < PRES_VALID_MAX)){
                    oc2[samp] = calc_c2(ocb_temp[samp], ocb_pres[samp], osatwvp[samp]); 
                     //Here setting the beta to zero.
                    obeta[samp] = 0;
                    odnc_ad[samp] = 1./K * 
                                (pow((otau[samp]/c0),3)) * 
                                (pow((1-0) * oc2[samp], 0.5)) * 
                                (pow(RHOL,2))/(pow(obe_lwp[samp],2.5));
                    if (odnc_ad[samp] > NDROP_MAX)
                       oqc_dnc_ad[samp]  = qc_set2(oqc_dnc_ad[samp], QC_INDETERM_NDROP_MAX);
                       
                    //There is no arscl data and so no cloud top and no cthick and no dnc
                    //may be this should be bad instead of indeterminate.
                    oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_BAD_NO_RETREIVAL);
                    oqc_dnc_tot_error[samp] = qc_set2(oqc_dnc_tot_error[samp], QC_BAD_NDROP_TOTERR_MISSING);
                    oqc_dnc[samp] = qc_set2(oqc_dnc[samp], QC_NO_CTH);
                } else {
                    // No valid pres 
                    oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_BAD_NO_RETREIVAL);
                    oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_BAD_NO_RETREIVAL);
                    oqc_dnc_tot_error[samp] = qc_set2(oqc_dnc_tot_error[samp], QC_BAD_NDROP_TOTERR_MISSING);
                    oqc_dnc[samp] = qc_set2(oqc_dnc[samp], QC_NO_CTH);
                }
            }
           }
          /* Ice conditions - not doing retrieval calculation */
        else {
                oqc_beta[samp] = qc_set2(oqc_beta[samp], QC_CB_ICE);
                oqc_lwp_ad[samp] = qc_set2(oqc_lwp_ad[samp], QC_CB_ICE);
                oqc_dnc_tot_error[samp] = qc_set2(oqc_dnc_tot_error[samp], QC_CB_ICE);
                oqc_dnc[samp] = qc_set2(oqc_dnc[samp], QC_CB_ICE);
                oqc_dnc_ad[samp] = qc_set2(oqc_dnc_ad[samp], QC_CB_ICE);
          }
      } //No tau and no be_lwp
    }

    /* ------------------------------------------------------------- */
    /* End algorithm */
    /* ------------------------------------------------------------- */

    /* Print the entire output dataset if debug level is 2 */
    if (dsproc_get_debug_level() > 1) {

        dsproc_dump_output_datasets(
            "./debug_dumps", "process_data.debug", 0);

    }
    dsproc_store_output_datasets();
    create_quicklooks(rundate2);
    return(1);
}




int qc_set2(int qc, int bit){
    if (bit == 0)
        qc =0;
    else
        qc |= ( 1<<(bit-1));
    return(qc);
}
int qc_good(int qc){
    if(qc == 0)
      return(1);
    else
      return(0);
}

float find_sonde_base_meas(float **invar, float cbh, float *ht_sonde, int nht, int samp){
/* Find the pressure or temp from sonde at a given cloud base*/
int i;
float base_meas;
i=1;
   while (i < nht){
        if (ht_sonde[i] > cbh ){
            base_meas = invar[samp][i-1];
            i=nht;
        }  
    i++;
    }
return(base_meas);
}

void create_quicklooks(char bdatestr[24]){
  char argument[1024], datadir[1024], qldir[1024], command[1024];
  char vapname[1024];
  int status, ipstat;
  pid_t pid;
  
  vapname[1023] = '\0';

  pid = fork();
  if (pid==0){
    sprintf(datadir, getenv("DATASTREAM_DATA"));
    sprintf(qldir, getenv("QUICKLOOK_DATA"));
    if ((datadir) && (qldir)){
        sprintf(vapname, "%s%s%s.%s", site,NDROPMFRSR_C1_DS_NAME, facility, NDROPMFRSR_C1_DS_LEVEL);
        
        sprintf(argument, 
                "/apps/tool/bin/dq_inspector_dmf -d %s%s%s.%s -s %s -p 100 -r %s -w %s -P",
                site, NDROPMFRSR_C1_DS_NAME, facility, NDROPMFRSR_C1_DS_LEVEL, 
                bdatestr, datadir, qldir);
        printf("The dq inspector argument is %s for the date %s\n", argument, bdatestr);
        system(argument);
        ipstat = dsenv_setenv("DATE","%s", bdatestr);
        if (ipstat != 1){
                DSPROC_DEBUG_LV1( "dsenv_setenv failed for DATE %s", bdatestr);
        }
    

        ipstat = dsenv_setenv("VAPNAME","%s%s%s.%s", site, NDROPMFRSR_C1_DS_NAME, facility, NDROPMFRSR_C1_DS_LEVEL);
        if (ipstat != 1){
                DSPROC_DEBUG_LV1( "dsenv_setenv failed for VAPNAME %s%s%s.%s", site,NDROPMFRSR_C1_DS_NAME, facility, NDROPMFRSR_C1_DS_LEVEL );
        }

        ipstat = dsenv_setenv("FACILITY","%s", facility);
        if (ipstat != 1){
                DSPROC_DEBUG_LV1( "dsenv_setenv failed for facility %s", facility);
        }
        ipstat = dsenv_setenv("SITE","%s", site);
        if (ipstat != 1){
                DSPROC_DEBUG_LV1( "dsenv_setenv failed for SITE %s", site);
        }
        
        /* Create the name of the batch file, as well as the command */
        /* Note that the output from IDL will go to stdout.          */
        /* No space after idl from C but need the space from command line*/

        sprintf(command, "%s/bin/idl", getenv("IDL_DIR"));
        sprintf(argument, "-em=%s/bytecode/%s_batch.sav", getenv("VAP_HOME"), "ndrop_mfrsr");


        /* Execute the IDL command.  If successful, the exit of the  */
        /* exec command will terminate the life of the child process */
        execl(command, "idl", argument, (char *)0);
        exit (127);
        DSPROC_DEBUG_LV1("The plots were plotted.\n");
    }
    else
        DSPROC_DEBUG_LV1("Could not find env for DATASTREAM_DATA and QUICKLOOK_DATA");
  }
  else {
    wait(&status);
  }
  if(status == 127) {
    DSPROC_DEBUG_LV1("The plots were not plotted.\n");
  }
}


float find_max_ref(float *ht_sonde, float clb, float clt, int nht){
/*Find the height between the cloud layer base and cloud layer top that corresponds
to the sonde height */
    int i;
    float max_ref;
    i=0;
    max_ref=0;
    while (i < nht-1){
        if ((ht_sonde[i] >= clb) && (ht_sonde[i+1] < clt)){
                max_ref = ht_sonde[i];
                i=nht;
        }  
    i++;
    }
    return(max_ref);

}

float calc_c2(float cb_temp, float cb_pres, float sat_vap_pres){
//Calculate c2
float rhoair, est, ws,c1,f1,f2,c2;
    rhoair = cb_pres/(GAS_CONSTANT *cb_temp);
    est = sat_vap_pres;
    ws = EPS * est/cb_pres;
    c1 = CP * cb_temp/(EPS *ALV);
    f1 = 1.-c1;
    f2 = pow((c1 + (ALV*ws*rhoair)/(cb_pres-est)), -1.);
    c2 = rhoair*GRAVITY*f1*f2*EPS*est*(pow(cb_pres-est, -2.0)); //Ask Sally
    
return c2;


}
float calc_sat_vap_pres(float temp){
//The input temperature should be in Kelvin
float ts, sr;
float ar, ar1, br, cr, dw, er, res, res1;
    ts = 373.16;
    sr = 3.0057166;
    ar = ts/temp;
    ar1 = ar -1.;
    br = 7.90298  * ar1;
    cr = 5.02808 * log10(ar);
    dw = 1.3816*.0000001 * (pow(10., (11.344 * (ar1/ar)) -1.));
    er = 8.1328*.001 * (pow(10.,(-3.49149 * ar1) -1.));
    res = pow(10, (cr-dw+er+sr-br));
    res1 = res*100.;
    return res1;


}
/**
 *  Main entry function.
 *
 *  @param  argc - number of command line arguments
 *  @param  argv - command line arguments
 *
 *  @return
 *    - 0 if successful
 *    - 1 if an error occurred
 */
int main(int argc, char **argv)
{
    int exit_value;
    int c;

    static const char *proc_names[] = {
        "ndrop_mfrsr"
    };

    int nproc_names = sizeof(proc_names)/sizeof(const char *);

    dsproc_use_nc_extension();

    dsproc_set_init_process_hook(init_process);
    dsproc_set_post_retrieval_hook(post_retrieval_hook);
    dsproc_set_process_data_hook(process_data);
    dsproc_set_finish_process_hook(finish_process);

    dsproc_set_pre_transform_hook(pre_transform_hook);
    dsproc_set_post_transform_hook(post_transform_hook);
 

    for (c=0; c < argc; c++)
    {
      if (strcmp(argv[c], "-b") == 0){
            begin_date_str[0] = (char) NULL;
            sprintf (begin_date_str, argv[c+1]);
            DSPROC_DEBUG_LV1("BeginDate is: %s\n", begin_date_str);
        }
      if (strcmp(argv[c], "-e") == 0){
	        end_date_str[0] = (char) NULL;
	        sprintf (end_date_str, argv[c+1]);
	        DSPROC_DEBUG_LV1 ("EndDate is: %s\n", end_date_str);
	    }

    }

    exit_value = dsproc_main(
        argc, argv,
        PM_TRANSFORM_VAP,
        gVersion,
        nproc_names,
        (const char **)proc_names);

    return(exit_value);
}


/*@}*/
