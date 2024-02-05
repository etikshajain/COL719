def main() :
    file_name = 'example.txt'
    with open(file_name, 'r',encoding='utf-8') as file:
        text = file.read()

    text = text.replace('\u2013', '\u002d')

    with open(file_name, 'w',encoding='utf-8') as file:
        file.write(text)

if __name__ == "__main__":
    main()