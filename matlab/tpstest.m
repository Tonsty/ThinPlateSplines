function r=tpstest()

N=5;
[yy,xx]=meshgrid(linspace(1,512,N),linspace(1,512,N));
s=[xx(:) yy(:)];

d=perturb(s);

t=ThinPlateSpline;
t.stiffness=1;
t.SourceSpaceKnots=s;
t.DestSpaceKnots=d;
disp(['Energy:' num2str(energy(t))]);

N=11;
[yy,xx]=meshgrid(linspace(1,512,N),linspace(1,512,N));
t.SourceSpaceQuery=[xx(:) yy(:)];

r=map(t);

beste=energy(t);
best=d;
tic;
for i=1:10000
  d=perturb(best);
  t.DestSpaceKnots=d;
  e=energy(t);
  if e<beste
    disp('accept');
    beste=e;
    best=map(t);
  end
end
toc
disp(beste);
plot(r(:,1),r(:,2),'g+');
hold on; plot(s(:,1),s(:,2),'.'); hold off;
hold on; plot(d(:,1),d(:,2),'rx'); hold off;
hold on; plot(best(:,1),best(:,2),'k.'); hold off;

  function r=perturb(x)
    %{
    dr=(x-256*ones(size(x)))./256;
    u=dr./repmat(sqrt(sum(dr.^2,2)),[1,2]);
    u(isnan(u))=0; 
    r=x+50.0.*u.*dr.^4;
    %}
    
    dr=(rand(size(x))-0.5).*50;
    r=x+dr;
    
  end
end