package tests

type T interface {
	Test() bool
}

type testCategories []testCatergory

func (tc testCategories) Test() bool {
	for _, c := range tc {
		c.runTests()
	}
	return countTestFail == 0
}

func Generate(optionalTest bool) T {
	t := testCategories{
		testGET,
		testDELETE,
		testIOMulti,
		testCgi,
		testAutoindex,
		testBadRequest,
		testLimitExpect,
		testLocation,
		testServerName,
		testReturn,
		testKeepAlive,
	}
	option := testCategories{
		testMultiConnection,
	}
	if optionalTest {
		t = append(t, option...)
	}
	return t
}
