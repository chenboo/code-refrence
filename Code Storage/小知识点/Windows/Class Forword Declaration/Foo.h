class CFoo
{
public:
	CFoo(int nx, int ny);

	int GetXvalue();
	int Getyvalue();
	void SetXvalue(int nx);
	void SetYvalue(int ny);

private:
	int m_nx;
	int m_ny;
};
