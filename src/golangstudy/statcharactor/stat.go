package main

import "fmt"

func calc(str string) (charCount, numCount, spaceCount, otherCount int) {

	utfChars := []rune(str)

	for i := 0; i < len(utfChars); i++ {

		if utfChars[i] >= 'a' && utfChars[i] <= 'z' || utfChars[i] >= 'A' && utfChars[i] <= 'Z' {
			charCount++
			continue
		}
		if utfChars[i] >= '0' && utfChars[i] <= '9' {
			numCount++
			continue
		}
		if utfChars[i] >= ' ' {
			spaceCount++
			continue
		}
		otherCount++
	}
	return
}

func main() {
	var str = "asdfafafasdfas887999   我爱你beijin"

	charCount, numCount, spaceCount, otherCount := calc(str)
	fmt.Printf("char count:%d num count%d sp count %d\n", charCount, numCount, spaceCount, otherCount)
	return
}
