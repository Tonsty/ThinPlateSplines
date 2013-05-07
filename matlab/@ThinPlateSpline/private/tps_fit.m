function ctx=tps_fit(ctx,dst,src,stiffness)
  % src and dst must be the same size
  % rows are different points, columns are dimension
  % have to transpose input matrices due to storage order
  mkaffine=@(v) [v ones(size(v,1),1)];
  assert(all(size(src)==size(dst)));
  if(ctx==0)
    ctx=tps_make(size(src,1),size(src,2));
  end
  tps_fit_mex(ctx,mkaffine(dst)',mkaffine(src)',single(stiffness));
end