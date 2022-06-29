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

func NormalTests() T {
	s := testCategories{
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
		testErrorPage,
	}
	return s
}

func OptionalTests() T {
	s := testCategories{
		testMultiConnection,
	}
	return s
}
