#define FADD(a,b) ( (a) + (b) )
#define FSUB(a,b) ( (a) - (b) )
#define FMUL(a,b,q) ( ((a) * (b)) >> (q) )
#define FDIV(a,b,q) ( ((a) << (q)) / (b) )

#define TOFIX(t,d,q) ( (t) ((d) * (double) (1ULL << (q))) )
#define FROMFIX(d,q) ((d) / (double) (1ULL << (q)))

typedef int32_t fix32;
typedef int16_t fix16;

#define QM 7
