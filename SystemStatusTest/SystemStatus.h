#include <pdh.h>
#include <pdhmsg.h>
#include <Iphlpapi.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "Iphlpapi.lib")

class CSystemStatus{
private:
		HQUERY					m_hQuery;

		size_t					m_CPUCoreCount;
		HCOUNTER				m_hCounterCPUTotal;	
		HCOUNTER*				m_phCounterCPUCore;		

		size_t					m_NetAdaptorCount;		//어댑터 갯수
		LPTSTR					m_lpNetAdaptorList;		//어댑터 이름 리스트
		HCOUNTER*				m_phCounterNetAdaptor;
		
public:
	CSystemStatus();
	~CSystemStatus(void);

	void	Init();
	void	Update();
	void	Terminate();
	
	size_t	getCPUCount() {	return m_CPUCoreCount;}
	void	getCPUStatus(LONG &total, LONG* arrCore, size_t arrCoreSize );
	void	getRAMStatus(int &availableMem, int &physicalMem); //단위:MB

	size_t			getNetAdaptorCount() { return m_NetAdaptorCount; }
	const LPTSTR	getNetAdatorList() { return m_lpNetAdaptorList;}
	void			getNETStatus(LONG *adapTotalByte, size_t adaptorCount);  

 };