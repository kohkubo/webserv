package tests

type T interface {
	Test()
}

type Categories []Catergory

func (cs Categories) Test() {
	for _, c := range cs {
		c.ExecuteTests()
	}
}

func Generate() T {
	s := Categories{
		GET,
	}
	return s
}
