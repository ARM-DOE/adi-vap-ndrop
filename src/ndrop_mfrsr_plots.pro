pro ndrop_mfrsr_plots, date,  $
                   data_home=data_home, qlook_home=qlook_home, vapname=vapname, $
                   site=site, facility=facility

@color_syms.include
device, decomposed=0
set_plot,'z'
set_resolution=[300,300]

!p.background=d_white
!p.color=d_black
!p.thick=2.5
!x.thick=2
!y.thick=2

;pngpath = '/home/sivaraman/www/dq_inspector/plots/sgp/sgpndropmfrsrC1.c1'
;pngpath = '/data/home/dev/vap/ndrop_mfrsr/DATA/www/process/vap/sgp/sgpndropmfrsrC1.c1'



 if(n_elements(data_home) eq 0) then data_home = getenv("DATASTREAM_DATA")
  if(n_elements(data_home) eq 0) then begin
     print, '   *** DATASTREAM_DATA is not defined - quitting ABNORMALLY'
     return
  endif


  if(n_elements(qlook_home) eq 0) then qlook_home = getenv("QUICKLOOK_DATA")
  if(n_elements(qlook_home) eq 0) then begin
      print, '  *** QLOOK_HOME is not defined - quitting ABNORMALLY'
      return
  endif

  if(n_elements(vapname) eq 0) then vapname = getenv ("VAPNAME")
  if(n_elements(vapname) eq 0) then begin
    print,'     *** Unspecified VAPNAME passed into ndrop_mfrsr_plots'
    return
  endif

  ;if(n_elements(site) eq 0) then begin site = getenv ("SITE")
  if(n_elements(site) eq 0) then begin
    print,'     ***Unspecified SITE passed into ndrop_mfrsr_plots'
    return
  endif

        ; we need to make sure we got a facility
        ; argument so that get_quicklook_dir will work properly

  if(n_elements(facility) eq 0) then begin
    print,'     ***Unspecified FACILITY passed into ndrop_mfrsr_plots'
    return
  endif



file= vapname
pngpath = qlook_home + '/' +  site + '/' +  vapname + '/'
print, pngpath
filename = data_home + '/' + site + '/' +  vapname + '/' + vapname + '*' + $
                string(format='(I0)',date) + '*nc'
print, filename
files = findfile(filename, count = count)
for ii=0, count -1 do begin
       filename = files(ii)
       fid = ncdf_open(filename)
       print, filename
       ncdf_varget, fid, 'drop_number_conc',ndrop
       ncdf_varget, fid, 'drop_number_conc_adiabatic',ndropa
       ncdf_varget, fid, 'drop_number_conc_toterror',ndrope
       ncdf_varget, fid, 'base_time', bt
       ncdf_varget, fid, 'time_offset', to
       time = bt+to

       systime2ymdhms,bt+to,yy,mm,dd,hh,nn,ss
       day = dd + hh/24. + nn/(24.*60) + ss/(24.*60*60)
         case mm[0] of
            1: mnth = ' Jan '
            2: mnth = ' Feb '
            3: mnth = ' Mar '
            4: mnth = ' Apr '
            5: mnth = ' May '
            6: mnth = ' Jun '
            7: mnth = ' Jul '
            8: mnth = ' Aug '
            9: mnth = ' Sep '
            10: mnth = ' Oct '
            11: mnth = ' Nov '
            12: mnth = ' Dec '
          endcase

        datestring1 = string(format='(I0)',dd) + mnth + string(format='(I0)',yy)
        datestring2 = string(format='(I0)',yy) + string(format='(I2.2)',mm) + $
                        string(format='(I2.2)',dd)
        systime2ymdhms, time, Pyyyy, Pmm, Pdd, Phh, Pnn, Pss
        hour = Phh + Pnn/60.0 + Pss/3600.0

         xtit = 'Hour of the day (UTC) '
         foo = systime(/utc)
         create_date1 = 'Creation date:'+ string(foo[n_elements(foo)-1])
         missing_string = 'All values equal -9999'
         mndrop = where(ndrop gt 0)
         mndropa = where(ndropa gt 0)
         mndrope = where(ndrope gt 0)

         plot,hour , ndrop, psym=2, xtitle =xtit, ytitle = 'Droplet number concentration (m^-3)',/nodata,color=d_black,  $
            charsize=1.2, xticklen=-0.02, yticklen=-0.01, title = file + ' for ' + datestring1[0], yr=[1,max(ndrop)], /ylog
         oplot,hour , ndrop, psym=2,color=d_blue
         xyouts,0.99,0.002,create_date1,/normal,alignment=1.0,charsize=0.9
         if (n_elements(mndrop) le 1) then begin
             xyouts,0.60,0.50,missing_string,/normal,alignment=1.0,charsize=0.9
         endif

        pngname = file + '.'+datestring2[0]+'.000000.drop_number_conc_log.png'
        year2 = string(format='(I0)',yy[0])
        mn2   =  string(format='(I2.2)',mm[0])
        dd2   = string(format='(I2.2)',dd[0])
        pngpath2 = pngpath + '/' + year2 + '/' + mn2 +'/' +  dd2 +'/'   + pngname
        saveimage, pngpath2

        plot,hour , ndropa, psym=2, xtitle =xtit, ytitle = 'Droplet number concentration Adiabatic(m^-3)',/nodata,color=d_black,  $
        charsize=1.2, xticklen=-0.02, yticklen=-0.01, title = file + ' for ' + datestring1[0], yr=[1,max(ndropa)], /ylog

        oplot,hour , ndropa, psym=2,color=d_blue

        xyouts,0.99,0.002,create_date1,/normal,alignment=1.0,charsize=0.9
        if (n_elements(mndropa) le 1) then begin
            xyouts,0.60,0.50,missing_string,/normal,alignment=1.0,charsize=0.9
        endif
        pngname = file + '.'+datestring2[0]+'.000000.drop_number_conc_adiabatic_log.png'
        year2 = string(format='(I0)',yy[0])
        mn2   =  string(format='(I2.2)',mm[0])
        dd2   = string(format='(I2.2)',dd[0])
        pngpath2 = pngpath + '/' + year2 + '/' + mn2 +'/' +  dd2 +'/'   + pngname
        saveimage, pngpath2

        plot,hour , ndrope, psym=2, xtitle =xtit, ytitle = 'Droplet number concentration Error Estimates (m^-3)',/nodata,color=d_black,  $
            charsize=1.2, xticklen=-0.02, yticklen=-0.01, title = file + ' for ' + datestring1[0], yr=[1,max(ndrope)], /ylog
        oplot,hour , ndrope, psym=2,color=d_blue
        xyouts,0.99,0.002,create_date1,/normal,alignment=1.0,charsize=0.9
        if (n_elements(mndrope) le 1) then begin
            xyouts,0.60,0.50,missing_string,/normal,alignment=1.0,charsize=0.9
        endif
        pngname = file + '.'+datestring2[0]+'.000000.drop_number_conc_error_log.png'
        year2 = string(format='(I0)',yy[0])
        mn2   =  string(format='(I2.2)',mm[0])
        dd2   = string(format='(I2.2)',dd[0])
        pngpath2 = pngpath + '/' + year2 + '/' + mn2 +'/' +  dd2 +'/'   + pngname
        saveimage, pngpath2
endfor

end
