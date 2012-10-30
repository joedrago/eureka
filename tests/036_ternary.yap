function a(x)
{
    return 2 + 2;
}

function b(x, y, z)
{
    return 2 + 3;
}

function cond()
{
    return 1;
}

print(3 ? 4 : 5);
print(0 ? 4 : 5);
print(cond() ? a(0) : b(1,2,3));
print(!cond() ? a(0) : b(1,2,3));

