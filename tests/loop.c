int g;
int change_g(int l){
	g += l;
	return g;
}
int sample(int a, int b){
	int i;
	for(i=0;i<a;i++)
	{
		change_g(b);
	}
	return 0+g;
}
