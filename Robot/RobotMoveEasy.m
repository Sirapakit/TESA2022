gen3 = loadrobot('kinovaGen3', 'DataFormat','row','Gravity',[0 0 -9.81]);
home = homeConfiguration(gen3)
n = 50;
for i = 1:n


myMatrix = [0 0 0 0 0 0 0];

myMatrix(:,:,i) = [i+.5 i+.5 i+.5 i+.5 i+.5 i+.5 i+.5];

%show(gen3,home)
show(gen3,myMatrix(:,:,i))
pause(.5)
end