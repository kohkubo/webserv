package checker

import (
	"fmt"
	"net/http"
	"net/http/httputil"
	"os"

	"github.com/google/go-cmp/cmp"
)

// "Connection"フィールドに関しては,
// http.ResponseではCloseというメンバーに設定されていたので
// TestInfoのHeader mapからConnectionだけ判定して, mapから削除
func NewResponseChecker2(info TestInfo2) reponseChecker {
	newResp := &Response2{}
	newResp.message = info.ExpectResponse
	return newResp
}

// 必要に応じてチェック項目(メンバー変数)を追加する
// http.Response参照
type Response2 struct {
	message string
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func (r Response2) Do(got *http.Response) (int, error) {
	var diff_flag int
	diff_checker := func(title string, x interface{}, y interface{}) {
		if diff := cmp.Diff(x, y); diff != "" {
			fmt.Fprintf(os.Stderr, "%s mismatch (-want +got):\n%s", title, diff)
			diff_flag++
		}
	}
	s, err := httputil.DumpResponse(got, true)
	if err != nil {
		return 0, err
	}
	diff_checker("message", r.message, string(s))
	return diff_flag, nil
}
