expect_capsule_equal <- function(actual,expected) {
    act <- quasi_label(rlang::enquo(actual),arg = "actual")


    comp <- waldo::compare(act$val$c1,expected$c1)
    expect(
        length(comp) == 0,
        sprintf("%s have wrong C1 center.\n\n%s",act$lab,paste0(comp,collapse = "\n\n"))
    )

    comp <- waldo::compare(act$val$c2,expected$c2)
    expect(
        length(comp) == 0,
        sprintf("%s have wrong C2 center.\n\n%s",act$lab,paste0(comp,collapse = "\n\n"))
    )

    comp <- waldo::compare(act$val$r1,expected$r1)
    expect(
        length(comp) == 0,
        sprintf("%s have wrong R1 radius.\n\n%s",act$lab,paste0(comp,collapse = "\n\n"))
    )

    comp <- waldo::compare(act$val$r2,expected$r2)
    expect(
        length(comp) == 0,
        sprintf("%s have wrong R2 radius.\n\n%s",act$lab,paste0(comp,collapse = "\n\n"))
    )

    invisible(act$val)
}
