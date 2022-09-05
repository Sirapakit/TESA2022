% transformation angle%
angle_x = [1,0,0,rx];
angle_y = [0,1,0,ry];
angle_z = [0,0,1,rz];
%convert angle rotation to homogeneous transformation%
tmhx = axang2tform(angle_x );
tmhy = axang2tform(angle_y);
tmhz = axang2tform(angle_z);
% indicate trajectory time
tInterval = [0 1];
tvec = 0:0.01:1;
%convert rotation matrix to homogeneous transformation%
rmh =trvec2tform([0 0 0]);
%create total homogeneous transformation%
for i= 1: length(n)
    ht = tmhx*tmhy*tmhz*rmh;
end
%Calculate interpolation%
[tfInterp, v1, a1] = transformtraj(t0,tF,tInterval,tvec);

rotations = tform2quat(tfInterp);
translations = tform2trvec(tfInterp);