function h=tps_make(nknots,ndim)
  h=tps_make_mex(int32(nknots),int32(ndim));
  if(h==0)
    exception=MException('ThinPlateSpline:Constructor',...
      'Error occured while trying to initialize context.');
    raise(exception);
  end
end