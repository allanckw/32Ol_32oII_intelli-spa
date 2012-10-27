#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
#include <vector>
class TestDesignExtractor: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
		CPPUNIT_TEST_SUITE(TestDesignExtractor); // Note 3 
		CPPUNIT_TEST(TestDesignExtractorBuilder);
		CPPUNIT_TEST_SUITE_END();
	
public:
	TestDesignExtractor(void);
	~TestDesignExtractor(void);
	void TestDesignExtractorBuilder();
};

