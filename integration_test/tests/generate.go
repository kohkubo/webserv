package tests

type T interface {
	Test()
}

type Categories []TestCatergory

func (cs Categories) Test() {
	for _, c := range cs {
		c.ExecuteTests()
	}
}

func Generate() T {
	s := Categories{
		GET,
		// ここに他のカテゴリーも追加していく
	}
	return s
}
