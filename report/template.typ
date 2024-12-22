#let project(
  title: "",
  abstract: [],
  authors: (),
  date: none,
  body,
) = {
  set document(author: authors.map(a => a.name), title: title)
  set page(numbering: "1", number-align: center, margin: (
    x: 1.75cm,
    top:1.75cm,
    bottom: 1.75cm,
  ))
  set text(font: "New Computer Modern", lang: "en")
  show link: underline
  show math.equation: set text(weight: 400)
  set heading(numbering: "1.1")

  align(center)[
    #block(text(weight: 700, 1.6em, title))
    #v(1em, weak: true)
    #date
  ]

  pad(
    top: 0.5em,
    bottom: 0.5em,
    x: 2em,
    grid(
      columns: (1fr,) * calc.min(3, authors.len()),
      gutter: 1em,
      ..authors.map(author => align(center)[
        *#author.name* \
        #author.email \
        #author.affiliation
      ]),
    ),
  )

  set par(justify: true)

  set align(center)
  heading(outlined: false, numbering: none, text(0.85em, smallcaps[Abstract]))
  block(
    width: 80%,
    abstract
  )
  set align(left)
  
  show: columns.with(2, gutter: 1.3em)
  body

  show bibliography: set heading(numbering: "1")
  bibliography("bib.yml", style: "iso-690-numeric")
}
