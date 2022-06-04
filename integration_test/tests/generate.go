package tests

type T interface {
	Test() bool
}

type testCategories []testCatergory

func (tc testCategories) Test() bool {
	for _, c := range tc {
		c.runTestCases()
	}
	return !IsFail() && !IsFatal()
}

func Generate() T {
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
	}
	return s
}
