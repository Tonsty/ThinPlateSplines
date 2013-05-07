function tps_update(ctx,dstknots,stiffness)
% Example: tps_update(obj.context,obj.DestSpaceKnots,obj.stiffness);
  mkaffine=@(v) [v ones(size(v,1),1)];
  tps_update_mex(ctx,mkaffine(dstknots)',stiffness);