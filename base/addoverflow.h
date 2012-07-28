#define addoverflow(r,a,b) ( b > 0 ? r<a:r>a )

#define addchkoverflow(r,a,b) ((r=a+b), addoverflow(r,a,b))
