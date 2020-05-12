int mult (int x, int y)
{
	return x * y;
}
int mult2 (int x, int y)
{
	asm ("mov %esi, %eax");
	asm ("imull %edi, %eax");
}
int main ()
{
	return mult(3, 4);
}
