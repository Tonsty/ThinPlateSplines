function dst=tps_eval(ctx,varargin)
  % dst=tps_eval(ctx,query)
  % dst=tps_eval(ctx) -- uses last query
  mkaffine=@(v) [v ones(size(v,1),1)];
  if(nargin==2)
    dst=tps_eval_mex(ctx,mkaffine(varargin{1})')';
  else
    dst=tps_eval_mex(ctx)';
  end
  dst=dst(:,1:(end-1));
end