package wholecheck

import (
	"fmt"
	"integration_test/httptest/client"
	"net/http"
	"net/http/httputil"
	"os"

	"github.com/google/go-cmp/cmp"
)

func NewResponseChecker(message string) client.ReponseChecker {
	newResp := &Checker{}
	newResp.message = message
	return newResp
}

// 必要に応じてチェック項目(メンバー変数)を追加する
// http.Response参照
type Checker struct {
	message string
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func (c Checker) Do(got *http.Response) (int, error) {
	var diff_flag int
	diff_checker := func(title string, x interface{}, y interface{}) {
		if diff := cmp.Diff(x, y); diff != "" {
			fmt.Fprintf(os.Stderr, "%s mismatch (-want +got):\n%s", title, diff)
			diff_flag++
		}
	}
	gotMessage, err := httputil.DumpResponse(got, true)
	if err != nil {
		return 0, err
	}
	diff_checker("message", c.message, string(gotMessage))
	return diff_flag, nil
}
