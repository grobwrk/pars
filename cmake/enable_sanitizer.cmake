function (enable_sanitizer target what)
    target_compile_options (
        ${target}
        PRIVATE
        -fsanitize=${what}
    )
    target_link_options (
        ${target}
        PRIVATE
        -fsanitize=${what}
    )
endfunction ()
