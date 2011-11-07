var a = 0;

a += 4;
a -= 4;
a *= 4;
a /= 4;

a &= 4;
a |= 4;
a ^= 4;
a <<= 4;
a >>= 4;

a = ~a;
a = a ^ a;
a = a & a;
a = a | a;
a = a << a;
a = a >> a;

#    YST_COMPOUND_ADD,                  // l+=r;
#    YST_COMPOUND_SUB,                  // l-=r;
#    YST_COMPOUND_MUL,                  // l*=r;
#    YST_COMPOUND_DIV,                  // l/=r;
#    YST_COMPOUND_BITWISE_AND,          // l&=r;
#    YST_COMPOUND_BITWISE_OR,           // l|=r;
#    YST_COMPOUND_BITWISE_XOR,          // l^=r;
#    YST_COMPOUND_SHIFTLEFT,            // l<<=r;
#    YST_COMPOUND_SHIFTRIGHT,           // l>>=r;
#    YST_BITWISE_NOT,                   // ~v
#    YST_BITWISE_XOR,                   // l^r
#    YST_BITWISE_AND,                   // l&r
#    YST_BITWISE_OR,                    // l|r
#    YST_SHIFTLEFT,                     // l<<r
#    YST_SHIFTRIGHT,                    // l>>r

