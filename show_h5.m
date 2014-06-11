
d=load("myfile.h5");
fn = fieldnames(d);

img1 = permute(d.img1,[2 3 1]);
d.img1=[];
img2 = permute(d.img2,[2 3 1]);
d.img2=[];

imwrite(uint8(img1), "img1.png")
imwrite(uint8(img2), "img2.png")

d
