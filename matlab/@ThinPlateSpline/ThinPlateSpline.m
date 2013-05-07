classdef ThinPlateSpline < handle
% TODO - gaurd dimensionality of input
% TODO - affirm that changing the number of source knots behaves reasonable
% TODO - add mex support and test
% TODO - document
% DONE - save/load sup
% FIXME - need to know nknots and ndim on context creation.  If these numbers change
%         the context should be deleted and re-created from scratch.
  properties(Hidden)
    context=uint64(0);
    have_fit=false;    % knots are commited to the context and a fit has been performed
    have_query=false;  % query points have been commited to the context
  end
  properties(SetObservable)
    SourceSpaceKnots = [];
    DestSpaceKnots   = [];
    stiffness        = 1.0;
    SourceSpaceQuery = [];
  end
  methods(Static,Access='public')
    function self=ThinPlateSpline(varargin)
      % obj=ThinPlateSpline()
      % obj=ThinPlateSpline(stiffness)
      % obj=ThinPlateSpline(stiffness,dst,src)
      addlistener(self,'SourceSpaceKnots','PostSet',@self.updateSrcKnots);
      addlistener(self,  'DestSpaceKnots','PostSet',@self.updateDstKnots);
      addlistener(self,       'stiffness','PostSet',@self.updateStiffness);
      addlistener(self,'SourceSpaceQuery','PostSet',@self.updateQuery);
    end
  end
  methods(Access='private')
    function sobj=saveobj(obj)
      sobj.stiffness        = obj.stiffness;
      sobj.SourceSpaceKnots = obj.SourceSpaceKnots;
      sobj.DestSpaceKnots   = obj.DestSpaceKnots;
      sobj.SourceSpaceQuery = obj.SourceSpaceQuery;
    end
    function obj=reload(obj,sobj)
      obj.stiffness        = sobj.stiffness;
      obj.SourceSpaceKnots = sobj.SourceSpaceKnots;
      obj.DestSpaceKnots   = sobj.DestSpaceKnots;
      obj.SourceSpaceQuery = sobj.SourceSpaceQuery;
    end
    
    function updateSrcKnots(obj,~,~)
      obj.SourceSpaceKnots=single(obj.SourceSpaceKnots);
      if(~isempty(obj.SourceSpaceKnots) &&...
          all(size(obj.SourceSpaceKnots)==size(obj.DestSpaceKnots)))
        obj.context=tps_fit(obj.context,obj.SourceSpaceKnots,obj.DestSpaceKnots,obj.stiffness);
        obj.have_fit=true;
        obj.updateQuery();
      else
        obj.have_fit=false;
      end
    end
    function updateDstKnots(obj,~,~)
      obj.DestSpaceKnots=single(obj.DestSpaceKnots);
      if(~isempty(obj.DestSpaceKnots) &&...
          all(size(obj.SourceSpaceKnots)==size(obj.DestSpaceKnots)))
        if(obj.have_fit)
          tps_update(obj.context,obj.DestSpaceKnots,obj.stiffness);
        else
          obj.context=tps_fit(obj.context,obj.SourceSpaceKnots,obj.DestSpaceKnots,obj.stiffness);
          obj.have_fit=true;
          obj.updateQuery();
        end
      end
    end
    function updateStiffness(obj,~,~)
      obj.stiffness=single(obj.stiffness);
      if(~isempty(obj.DestSpaceKnots) &&...
          all(size(obj.SourceSpaceKnots)==size(obj.DestSpaceKnots)))
        tps_update(obj.context,obj.DestSpaceKnots,obj.stiffness);
      end
    end
    function updateQuery(obj,varargin)
      obj.SourceSpaceQuery=single(obj.SourceSpaceQuery);
      if(~isempty(obj.SourceSpaceQuery))
        if(obj.have_fit)
          tps_eval(obj.context,obj.SourceSpaceQuery);
          obj.have_query=true;
        end
      end
    end
    function delete(obj)
      obj.context=tps_free(obj.context);
    end
  end
  methods(Static,Access='private')
    function obj=loadobj(sobj)
      obj=ThinPlateSpline;
      reload(obj,sobj);
    end
  end
  methods
    function obj=fit(obj,dst,src)
      obj.DestSpaceKnots=dst;
      obj.SourseSpaceKnots=src;
    end
    function obj=update(obj,dst)
      obj.DestSpaceKnots=dst;
    end
    function dst=map(obj,varargin)
      % dst=map(obj)
      % dst=map(obj,src)
      if(nargin>1)
        obj.SourceSpaceQuery=varargin{2};
      end
      dst=[]; %TODO should raise an exception and give some help
      if(obj.have_fit && obj.have_query)
        dst=tps_eval(obj.context);
      end
    end
    function e=energy(obj)
      %TODO if not have_fit, should raise an exception and give some help
      e=tps_energy(obj.context);
    end
  end
end