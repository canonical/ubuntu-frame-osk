project(
    'list-layouts',
    'c',
    default_options : [
        'warning_level=2',
        'c_std=c11',
    ]
)

gnome_desktop = dependency('gnome-desktop-3.0')
executable(
    'list-layouts',
    'main.c',
    install : true,
    install_dir: '/usr/bin',
    dependencies : [gnome_desktop],
)
