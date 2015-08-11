; Abstract:
;       This is the top-level IDL commands needed to run quicklook plot
;    scripts for the NDROP VAP.  Environment variables, set by the C-code
;    portion of the VAP, are how the arguments are transferred to IDL.
;
; Author:
;     Chitra Sivaraman
; Date Created:
;     October, 2009
;
;-

pro ndrop_mfrsr_batch
  ;data_home='/home/sivaraman/dev/vap/src/ndrop_mfrsr/DATA/data/datastream'
  ;qlook_home ='/data/home/dev/vap/ndrop_mfrsr/DATA/www/process/vap'
  ;vapname ='sgpndropmfrsrC1.c1'
  ;site ='sgp'
  ;facility ='C1'
  ;date= '20100101'
  date = long(getenv("DATE"))
  vapname = getenv("VAPNAME")
  site = getenv("SITE")
  facility = getenv ("FACILITY")
  ndrop_mfrsr_plots, date, data_home=data_home, qlook_home=qlook_home,  vapname=vapname, site=site, facility=facility

  return

end

