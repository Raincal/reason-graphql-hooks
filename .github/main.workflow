workflow "Build and Publish" {
  on = "push"
  resolves = ["Publish"]
}

action "Build" {
  uses = "actions/npm@master"
  args = "install"
}

action "Tag" {
  uses = "actions/bin/filter@master"
  needs = ["Build"]
  args = "tag v*"
}

action "Publish" {
  uses = "actions/npm@master"
  needs = ["Tag"]
  args = "publish --access reason-graphql-hooks/public"
  secrets = ["NPM_AUTH_TOKEN"]
}
