package tests

type T interface {
	Test() bool
}

type Categories []TestCatergory

func (cs Categories) Test() bool {
	for _, c := range cs {
		c.ExecuteTests()
	}
	return !IsFail() && !IsFatal()
}

func Generate() T {
	s := Categories{
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
