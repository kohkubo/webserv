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

// ひとまずインターフェース(呼び出し元ではTestメソッドだけ呼び出せば良いので)
func Generate() T {
	s := Categories{
		GET,
		// ここに他のカテゴリーも追加していく
	}
	return s
}
