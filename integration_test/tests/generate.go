package tests

type T interface {
	Test() bool
}

type testCategories []testCatergory

func (cs testCategories) Test() bool {
	for _, c := range cs {
		c.ExecuteTests()
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
