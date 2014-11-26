set (UNIVERSALCHARDET_SOURCES
	CharDistribution.cpp
	JpCntx.cpp
	LangBulgarianModel.cpp
	LangCyrillicModel.cpp
	LangGreekModel.cpp
	LangHebrewModel.cpp
	LangHungarianModel.cpp
	LangThaiModel.cpp
	nsBig5Prober.cpp
	nsCharSetProber.cpp
	nsEscCharsetProber.cpp
	nsEscSM.cpp
	nsEUCJPProber.cpp
	nsEUCKRProber.cpp
	nsEUCTWProber.cpp
	nsGB2312Prober.cpp
	nsHebrewProber.cpp
	nsLatin1Prober.cpp
	nsMBCSGroupProber.cpp
	nsMBCSSM.cpp
	nsSBCharSetProber.cpp
	nsSBCSGroupProber.cpp
	nsSJISProber.cpp
	nsUniversalDetector.cpp
	nsUTF8Prober.cpp
)

set (UNIVERSALCHARDET_HEADERS
	CharDistribution.h
	JpCntx.h
	nsBig5Prober.h
	nsCharSetProber.h
	nsCodingStateMachine.h
	nsEscCharsetProber.h
	nsEUCJPProber.h
	nsEUCKRProber.h
	nsEUCTWProber.h
	nsGB2312Prober.h
	nsHebrewProber.h
	nsLatin1Prober.h
	nsMBCSGroupProber.h
	nsPkgInt.h
	nsSBCharSetProber.h
	nsSBCSGroupProber.h
	nsSJISProber.h
	nsUniversalDetector.h
	nsUTF8Prober.h
)

set (UNIVERSALCHARDET_DEFINITIONS "" PARENT_SCOPE)
set (UNIVERSALCHARDET_INCLUDES ${UNIVERSALCHARDET_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set (UNIVERSALCHARDET_SOURCES ${UNIVERSALCHARDET_SOURCES} PARENT_SCOPE)
set (UNIVERSALCHARDET_HEADERS ${UNIVERSALCHARDET_HEADERS} PARENT_SCOPE)
set (UNIVERSALCHARDET_LIBRARIES "" PARENT_SCOPE)
