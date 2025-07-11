#ifndef PATCH_LLC_H
#define PATCH_LLC_H

void extended_llc_init(void);

void rom_llc_set_start_ext_adv_fragment_callback(api_llp_start_ext_adv_fragment callback);

EN_LLP_ERR_T rom_llc_start_ext_adv_fragment(stLlpExtAdvFragmentParam_t *pstParam);

void rom_llc_set_start_ext_scan_fragment_callback(api_llp_start_ext_scan_fragment callback);

EN_LLP_ERR_T rom_llc_start_ext_scan_fragment(stLlpExtScanFragmentParam_t *pstParam);

void rom_llc_set_start_ext_init_fragment_callback(api_llp_start_ext_init_fragment callback);

EN_LLP_ERR_T rom_llc_start_ext_init_fragment(stLlpExtInitFragmentParam_t *pstParam);

#endif
